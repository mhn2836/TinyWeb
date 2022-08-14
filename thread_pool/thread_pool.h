#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#include<iostream>
#include<string>
#include<cstring>
#include<exception>
#include<list>

#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<cassert>
#include<pthread.h>


#include "../locker/locker.h"
#include "sql_pool.h"

template<class t>
class thread_pool{
public:
    thread_pool(int thread_num, int max_request, sql_pool *sql_pool);
    ~thread_pool();
    bool append(t *request);
    bool append(t *request, int state);

private:
    int _thread_num;
    int _max_request;
    pthread_t *_thread;
    std::list<t*> _queue;
    locker _mutex;
    sem _sem;

    sql_pool *_sql_pool;

private:
    static void *work_func(void *arg){
        return arg;
    }
    void run();

};

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
        if(pthread_detach(_thread[i])){
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

#endif