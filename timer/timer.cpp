#include "timer.h"
#include "../http_conn/http_conn.h"

//util_timer
util_timer::util_timer(int delay):_expire(time(NULL) + delay){

}

util_timer::~util_timer(){}


//heap_timer
heap_timer::heap_timer(){
    _heap.reserve(1);
}

heap_timer::heap_timer(int capacity):_capacity(capacity), _cur_size(0){
    _heap.reserve(_capacity);

    if(_heap.empty()) throw std::exception();

    for(int i = 0;i < _capacity; i++){
        _heap.emplace_back(nullptr);
    }
}   

heap_timer::heap_timer(int cap, int size, std::vector<util_timer*> timer):_cur_size(size), _capacity(cap){
    if(_capacity < _cur_size) throw std::exception();

    _heap.reserve(_capacity);
    for(int i = 0;i < _capacity; i++){
        _heap.emplace_back(nullptr);
    }

    if(_cur_size){
        for(int i = 0;i < _cur_size; i++) _heap[i] = timer[i];
        //初始化小根堆
        for(int i = _cur_size / 2 - 1;  i >= 0; i--){
            heap_adjust(i);
        }

    }
}

heap_timer::~heap_timer(){
    for(int i = 0; i < _cur_size; i++){
        delete _heap[i];
    }

    _heap.clear();
}

void heap_timer::add_timer(util_timer *timer){
    if(timer){
        if(_cur_size >= _capacity) {resize();}
        int temp = _cur_size ++;
        int father = 0;

        //更新小顶堆
        for(; temp > 0; temp = father){
            father = (temp - 1) / 2;
            if(_heap[father]->_expire <= timer->_expire) break;

            _heap[temp] = _heap[father];
        }

        _heap[temp] = timer;
    }
}

void heap_timer::del_timer(util_timer *timer){
    if(timer){
        timer->cb_func= NULL;//节省定时器销毁的损耗


    }
}

void heap_timer::pop_timer(){
    if(_cur_size){
        if(_heap[0]){
            delete _heap[0];
            _heap[0] = _heap[--_cur_size];
            //更新小顶堆
            heap_adjust(0);
        }
    }
}

void heap_timer::heap_adjust(int begin){
    int father = begin;
    int son = father * 2 + 1;

    while(son < _cur_size){
        if(son + 1 < _cur_size && _heap[son]->_expire > _heap[son + 1]->_expire)
        son++;

        if(_heap[son]->_expire < _heap[father]->_expire){
            util_timer* temp = _heap[father];
            _heap[father] = _heap[son];
            _heap[son] = temp;

            father = son;
            son = father * 2 + 1;
        }
        else break;
    }
}

util_timer* heap_timer::top(){
    //if(!_cur_size) throw std::exception();
    return _cur_size == 0 ? nullptr : _heap[0];
}

bool heap_timer::empty(){
    return _cur_size == 0;
}

void heap_timer::resize(){
    _heap.reserve(2 * _capacity);

    for(int i = 0;i < _capacity; i++){
        _heap.emplace_back(nullptr);
    }

    _capacity = 2 * _capacity;
}

void heap_timer::tick(){
    util_timer *temp = _heap[0];

    while(_cur_size){
        if(!temp) break;
        if(temp->_expire > time(NULL)){
            break;
        }
        if(_heap[0]->cb_func) _heap[0]->cb_func(_heap[0]->_data);

        pop_timer();
        temp = _heap[0];
    }
}

//util
void util::init(int timeslot){
    _timeslot = timeslot;
}

//设置fd非阻塞
int util::set_non_blocking(int fd){
    int old_opt = fcntl(fd, F_GETFL);
    int new_opt = old_opt | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_opt);

    return old_opt;
}

//内核时间表注册读时间，开启EPOLLONTSHOT和ET模式
void util::add_fd(int efd, int fd, bool oneshot,int trig_mode){
    epoll_event event;
    event.data.fd =fd;
    if(trig_mode == 1) event.events = EPOLLIN || EPOLLET || EPOLLRDHUP;
    else event.events = EPOLLIN || EPOLLRDHUP;

    if(oneshot) event.events |= EPOLLONESHOT;

    epoll_ctl(efd, EPOLL_CTL_ADD, fd, &event);
    set_non_blocking(fd);
}

void util::sig_handler(int sig){
    int save_errno = errno;
    int msg = sig;
    send(_pipefd[1], (char*)&msg, 1, 0);
    errno = save_errno;
}

void util::add_sig(int sig, void(handler)(int), bool restart = true){
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));

    sa.sa_handler = handler;

    if(restart){
        sa.sa_flags |= SA_RESTART;
    }
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != 1);
}

void util::timer_handler(){
    _heap_timer.tick();

    if(_heap_timer.empty()){
        alarm(_timeslot);
    }
    //将堆的顶部定时器超时时间作为下一次tick()函数的周期时间
    else alarm(_heap_timer.top()->_expire - time(NULL));
}

void util::show_error(int cfd, const char *info){
    send(cfd, info ,strlen(info), 0);
    close(cfd);
}

int *util::_pipefd = 0;
int util::_efd = 0;

void cb_func(data *user_data){
    epoll_ctl(util::_efd, EPOLL_CTL_DEL, user_data->sockfd, 0);
    assert(user_data);
    close(user_data->sockfd);
    http_conn::m_user_count--;
    }