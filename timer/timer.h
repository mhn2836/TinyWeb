#ifndef TIMER_H
#define TIMER_H

#include<iostream>
#include<string>
#include<cstring>
#include<exception>
#include<list>
#include<vector>

#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<cassert>
#include<pthread.h>
#include<sys/time.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<netinet/in.h>

class util_timer;
const int buf_size = 64;

struct data
{
    sockaddr_in addr;
    util_timer *timer;
    int sockfd;
    char buffer[buf_size];
};

class util_timer{
public:
    util_timer(int delay);
    ~util_timer();

    time_t _expire;//超时时间
    data _data;

    void (*cb_func)(data);

};

class heap_timer{
public:
    heap_timer(int cap);
    ~heap_timer();

private:
    data _data;

    std::vector<util_timer*> _heap;
    int _capacity;
    int _cur_size;

public:
    void add_timer(util_timer *timer);
    void del_timer(util_timer *timer);
    void update_timer();
};


#endif