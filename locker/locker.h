#ifndef LOCKER_H
#define LOCKER_H

#include<iostream>
#include<exception>
#include<pthread.h>
#include<semaphore.h>

//封装互斥锁
class locker{
public:
    locker(){
        if(pthread_mutex_init(&_mutex, NULL)) throw std::exception();
    }

    ~locker(){
        pthread_mutex_destroy(&_mutex);
    }

    bool mutex_lock(){
        return pthread_mutex_lock(&_mutex) == 0;
    }

    bool mutex_unlock(){
        return pthread_mutex_unlock(&_mutex) == 0;
    }

    pthread_mutex_t *mutex_get(){
        return &_mutex;
    }

private:
    pthread_mutex_t _mutex;
};

//封装信号量
class sem{
public:
    sem(){
        if(sem_init(&_sem, 0, 0)) throw std::exception();
    }

    sem(int sem_num){
        if(sem_init(&_sem, 0, sem_num)) throw std::exception();
    }

    ~sem(){
        sem_destroy(&_sem);
    }

    bool wait(){
        return sem_wait(&_sem) == 0;
    }

    bool post(){
        return sem_post(&_sem) == 0;
    }

    
private:
    sem_t _sem;
};

//封装条件变量
class conn{
public:
    conn(){
        if(pthread_cond_init(&_cond, NULL)) throw std::exception();
    }

    ~conn(){
        pthread_cond_destroy(&_cond);
    }

    bool cond_wait(pthread_mutex_t *mutex){
        return pthread_cond_wait(&_cond, mutex) == 0;
    }

    bool time_wait(pthread_mutex_t *mutex, struct timespec t){
        return pthread_cond_timedwait(&_cond, mutex, &t);
    }

    bool signal(){
        return pthread_cond_signal(&_cond) == 0;
    }

    bool broadcast(){
        return pthread_cond_broadcast(&_cond) == 0;
    }

private:
    pthread_cond_t _cond;
};

#endif