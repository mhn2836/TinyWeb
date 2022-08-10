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
#include<signal.h>
#include<sys/epoll.h>

#include "../log/log.h"
#include "../http_conn/http_conn.h"

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
    data *_data;

    void (*cb_func)(data*);

};

class heap_timer{
public:
    heap_timer();
    heap_timer(int cap);
    heap_timer(int cap, int size, std::vector<util_timer*> timer);
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
    void pop_timer();

    void heap_adjust(int begin);
    util_timer *top();
    bool empty();

    void resize();
    void tick();
};

class util{
public:
    util(){
    }

    ~util(){

    }

    void init(int timeslot);

    int set_non_blocking(int fd);

    void add_fd(int efd, int fd, bool one_shot, int trig_mode);

    static void sig_handler(int sig);

    void add_sig(int sig, void(handler)(int), bool restart = true);

    void timer_handler();

    void show_error(int cfd, const char *info);
public:
    static int *_pipefd;
    heap_timer _heap_timer;
    static int _efd;
    int _timeslot;
};

int *util::_pipefd = 0;
int util::_efd = 0;

void cb_func(data*);

#endif