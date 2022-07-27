#ifndef THREAD_POOL_H
#define THREAD_POOL_H

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


#endif