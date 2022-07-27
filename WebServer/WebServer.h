#ifndef _WEBSERVER_H
#define _WEBSERVER_H

#include<iostream>
#include<string>
#include<cstring>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<netinet/in.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<cassert>

#include "../http_conn/http_conn.h"


const int MAX_FD = 65536;
const int MAX_EVENT_NUM = 10000;
const int TIMESLOT = 5;

class WebServer{
public:
    WebServer(
        int port, std::string user, std::string passwd, std::string dbname, int log_write,
        int opt_linger, int trigger_mode, int sql_num, int thread_num, int close_log, 
        int actor_model
    );
    ~WebServer();

private:
    int _port;
    int _logwrite;
    int _optlinger;
    int _triggermode;
    int _sqlnum;
    int _threadnum;
    int _closelog;
    int _actormodel; 

public:
    //写日志
    void log_write();
    //初始化socket连接
    void init_socket();
    //触发模式
    void trig_mode();
    //数据库线程池
    void sql_pool();
    //线程池
    void thread_pool();
    //epoll-event
    void epoll_ev();
    
private:
    char *_root;
    http_conn *users;


};


#endif 