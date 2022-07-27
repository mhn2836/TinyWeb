#include "timer.h"

//util_timer
util_timer::util_timer(int delay):_expire(delay){

}

util_timer::~util_timer(){}


//heap_timer
heap_timer::heap_timer(int capacity):_capacity(capacity), _cur_size(0){
    _heap.reserve(buf_size);

    if(_heap.empty()) throw std::exception();

    for(int i = 0;i < _capacity; i++){
        _heap[i] = 0;
    }
}   

heap_timer::~heap_timer(){
    _heap.clear();
}

void heap_timer::add_timer(util_timer *timer){
    if(timer){
        if(_cur_size >= _capacity) {}
        int temp = _cur_size + 1;
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