#ifndef LOG_H
#define LOG_H

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
#include<pthread.h>
#include<stdarg.h>

#include "block_queue.h"

//懒汉线程单例模式
class log{
public:
    static log *get_instance(){
        static log instance;
        return &instance;
    }
private:
    log();
    ~log();
    void *async_write_log();

public:
    static void *flush_log(void *arg){
        log::get_instance() -> async_write_log();
        return arg;
    }

    bool init(const char* filename, int close_log, int log_buffer = 8192,
    int split_lines = 5000000, int max_queue = 0);

    void write_log(int level, const char *format, ...);

    void flush();
private:
    char _dir[128];
    char _log[128];
    int _split_lines;
    int _log_buffer;
    long long _lines_count;
    int _day;
    char *_buffer;
    FILE *_fp;
    
    bool _is_async;
    int _close_log;

    locker _mutex;
    block_queue<std::string> *_log_queue;
};

#define LOG_DEBUG(format, ...) if(!_close_log) {log::get_instance()->write_log(0, format, ##__VA_ARGS__); log::get_instance()->flush();}
#define LOG_INFO(format, ...) if(!_close_log) {log::get_instance()->write_log(1, format, ##__VA_ARGS__); log::get_instance()->flush();}
#define LOG_WARN(format, ...) if(!_close_log) {log::get_instance()->write_log(2, format, ##__VA_ARGS__); log::get_instance()->flush();}
#define LOG_ERROR(format, ...) if(!_close_log) {log::get_instance()->write_log(3, format, ##__VA_ARGS__); log::get_instance()->flush();}
#endif