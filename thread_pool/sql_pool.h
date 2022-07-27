#ifndef SQL_POOL_H
#define SQL_POOL_H

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
#include<mysql/mysql.h>

#include "../locker/locker.h"
#include "../log/log.h"

class sql_pool{
public:
    static sql_pool *get_instance();

    void init(std::string url, std::string user, std::string password, std::string dbname,
    int port, int max_conn, int close_log);
    
    MYSQL *get_pool_conn();
    bool release(MYSQL *conn);
    int free_conn();
    void destroy_pool();
    
private:
    sql_pool():_used_conn(0), _free_conn(0){}
    ~sql_pool(){destroy_pool();}

public:
    std::string _url;
    std::string _port;
    std::string _user;
    std::string _password;
    std::string _dbname;
    int _close_log;

private:
    int _max_conn;
    int _used_conn;
    int _free_conn;
    std::list<MYSQL*> _conn_list;

    locker _mutex;
    sem _sem;
};

class RAII_conn{
public:
    RAII_conn(MYSQL **SQL, sql_pool *pool){
        *SQL = pool->get_pool_conn();
        _conn_RAII = *SQL;
        _pool_RAII = pool;
    }
    ~RAII_conn(){
        _pool_RAII->release(_conn_RAII);
    }
private:
    MYSQL *_conn_RAII;
    sql_pool *_pool_RAII;
};

#endif