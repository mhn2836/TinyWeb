#ifndef BLOCK_QUEUE_H
#define BLOCK_QUEUE_H

#include<iostream>
#include<string>
#include<cstring>

#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<sys/time.h>

#include "../locker/locker.h"


template<class t>
class block_queue{
public:
    block_queue(int max_size):_queue_size(0), _front(-1), _back(-1){
        if(max_size <= 0) exit(-1);
        
        _max_size = max_size;
        _array = new t[max_size];
    }

    ~block_queue(){
        _mutex.mutex_lock();
        if(_array) delete []_array;
        _mutex.mutex_unlock();
    }

private:
    int _max_size;
    int _queue_size;
    int _front;
    int _back;
    t *_array;

    locker _mutex;
    conn _cond;

public:
    void clear(){
        _mutex.mutex_lock();
        _queue_size = 0;
        _front = -1;
        _back = -1;
        _mutex.mutex_unlock();
    }

    bool empty(){
        _mutex.mutex_lock();
        int qsize = _queue_size;
        _mutex.mutex_unlock();
        return qsize == 0;
    }

    bool full(){
        _mutex.mutex_lock();
        int qsize = _queue_size;
        _mutex.mutex_unlock();
        return _max_size <= qsize;
    }

    t& front(){
        _mutex.mutex_lock();
        if(!_queue_size) {
            _mutex.mutex_unlock();
            throw std::exception();
        }
        t temp = _array[_front];
        _mutex.mutex_unlock();

        return temp;
    }

    t& back(){
        _mutex.mutex_lock();
        if(_max_size <= _queue_size) {
            _mutex.mutex_unlock();
            throw std::exception();
        }
        t temp = _array[_back];
        _mutex.mutex_unlock();

        return temp;
    }

    int size(){
        int temp = 0;
        _mutex.mutex_lock();
        temp = _queue_size;
        _mutex.mutex_unlock();

        return temp;
    }

    
    void push(const t &data){
        _mutex.mutex_lock();
        if(_queue_size >= _max_size){
            //_cond.broadcast();
            _cond.cond_wait(_mutex.mutex_get());
            //队列数据满的情况下阻塞，等待其他线程发出的满足条件变量的信号
            _mutex.mutex_unlock();
            return;
        }
        _back = (_back + 1) % _max_size;
        _queue_size++;
        _array[_back] = data;
        //push操作中发出条件信号，唤醒所有阻塞在条件变量下的线程
        _cond.broadcast();
        _mutex.mutex_unlock();
    }

    bool pop(t &data){
        _mutex.mutex_lock();
        while(_queue_size <= 0){
            //队列中无数据的情况下阻塞，等待其他线程发出的满足条件变量的信号
            if(!_cond.cond_wait(_mutex.mutex_get())){
                _mutex.mutex_unlock();
                return false;
            }
        }

        _front = (_front + 1) % _max_size;
        _queue_size--;
        data = _array[_front];
        _cond.broadcast();
        _mutex.mutex_unlock();
        return true;
    }

    bool pop(t &data, int timeout){
        struct timespec ti ={0, 0};
        struct timeval now ={0, 0};
        gettimeofday(&now, NULL);

        _mutex.mutex_lock();
        if(_queue_size <= 0){
            ti.tv_sec = now.tv_sec + timeout / 1000;
            ti.tv_nsec = (timeout % 1000) * 1000;
            if(!_cond.time_wait(_mutex.mutex_get(), ti)){
                _mutex.mutex_unlock();
                return false;
            }
        }
        if(_queue_size <= 0){
            _mutex.mutex_unlock();
            return false;
        }

        _front = (_front + 1) % _max_size;
        _queue_size--;
        data = _array[_front];
        _cond.broadcast();
        _mutex.mutex_unlock();

        return true;
    }
};

#endif