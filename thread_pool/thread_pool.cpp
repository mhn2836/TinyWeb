#include "thread_pool.h"

template<class t>
thread_pool<t>::thread_pool(int thread_num, int max_request, sql_pool *sql_pool):_thread_num(thread_num), 
_max_request(max_request), _thread(nullptr), _sql_pool(sql_pool){
    if(thread_num <= 0 || max_request <= 0){throw std::exception();}
    _thread = new pthread_t[_thread_num];

    if(!_thread){throw std::exception();}

    for(int i = 0;i < thread_num; i++){
        if(pthread_create(_thread + i, NULL, work_func, this)){
            delete []_thread;
            throw std::exception();
        }
        if(pthread_detach(_thread)){
            delete []_thread;
            throw std::exception();
        }
    }
}

template<class t>
thread_pool<t>::~thread_pool(){
    delete []_thread;
}

template<class t>
bool thread_pool<t>::append(t *request){
    _mutex.mutex_lock();

    if(_queue.size() >= _max_request){
        _mutex.mutex_unlock();
        return false;
    }

    _queue.push_back(request);
    _mutex.mutex_unlock();

    _sem.post();
    return true;
}

template<class t>
bool thread_pool<t>::append(t *request, int state){
    _mutex.mutex_lock();

    if(_queue.size() >= _max_request){
        _mutex.mutex_unlock();
        return false;
    }

    request -> m_state = state;
    _queue.push_back(request);
    _mutex.mutex_unlock();

    _sem.post();
    return true;
}

template<class t>
void thread_pool<t>::run(){
    while(1){
        _sem.wait();
        _mutex.mutex_lock();

        if(_queue.empty()){
            _mutex.mutex_unlock();
            continue;
        }

        t *request = _queue.front();
        _queue.pop_front();
        _mutex.mutex_unlock();

        if(!request) continue;
        
        //proactor模式
        RAII_conn mysql_conn(&request->mysql, _sql_pool);
        request -> process();
    }

}