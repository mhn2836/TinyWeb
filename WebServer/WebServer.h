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
#include "../thread_pool/thread_pool.h"


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

public:
    void init(int port, std::string user, std::string passwd, std::string dbname, int log_write,
        int sql_num, int thread_num, int close_log
    );

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

    //timer相关
    void timer(int cfd, struct sockaddr_in client_address);

    void adjust_timer(util_timer *timer);

    void deal_timer(util_timer *timer, int sockfd);

    bool deal_clinet_data();

    bool deal_with_signal(bool &timeout, bool &stop_server);

    void deal_with_read(int sockfd);

    void deal_with_write(int sockfd);
    
private:
    char *_root;
    int _port;
    int _log_write;
    int _close_log;
    int _actormodel;

    int _pipe[2];
    int _efd;
    http_conn *_users;

    sql_pool *_sql_pool;
    std::string _user;
    std::string _password;
    std::string _dbname;
    int _sql_num;

    thread_pool<http_conn> *_pool;
    int _thread_num;

    epoll_event _events[MAX_EVENT_NUM];

    int _listenfd;
    int _OPT_LINGER;
    int _trig_mode;
    int _listen_mode;
    int _conn_mode;

    data *_user_timer;
    util _utils;


};


#endif 