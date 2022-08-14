#include "sql_pool.h"

sql_pool *sql_pool::get_instance(){
    static sql_pool instance;
    return &instance;
}

void sql_pool::init(std::string url, std::string user, std::string password, std::string dbname,
int port, int max_conn, int close_log){
    _url = url; _user = user;_password = password;_dbname = dbname;_port = port;close_log = close_log;

    for(int i = 0; i < max_conn; i++){
        MYSQL *conn = NULL;
        MYSQL *ret = NULL;
        ret = mysql_init(conn);

        if(!ret){
            LOG_ERROR("MySQL Error: mysql_init() returns NULL");
            exit(1);
        }
        else conn = ret;

        ret = mysql_real_connect(conn, url.c_str(), user.c_str(), password.c_str(), 
        dbname.c_str(), port, NULL, 0);

        if(!ret){
            std::string err_info( mysql_error(conn) );
			err_info = (std::string("MySQL Error[errno=")
				+ std::to_string(mysql_errno(conn)) + std::string("]: ") + err_info);
			LOG_ERROR( err_info.c_str() );
			exit(1);
        }
        else conn = ret;

        _conn_list.push_back(conn);
        _free_conn++;
    }

    _sem = sem(_free_conn);

    _max_conn = _free_conn;
}

MYSQL * sql_pool::get_pool_conn(){
    MYSQL *conn = NULL;
    if(!_conn_list.size()) return NULL;
    
    _sem.wait();
    _mutex.mutex_lock();

    conn = _conn_list.front();
    _conn_list.pop_front();
    _free_conn--;
    _used_conn++;

    _mutex.mutex_unlock();
    return conn;
}

bool sql_pool::release(MYSQL *conn){
    if(!conn) return false;

    _mutex.mutex_lock();

    _conn_list.push_back(conn);
    _free_conn++;
    _used_conn--;

    _mutex.mutex_unlock();
    _sem.post();

    return true;
}

int sql_pool::free_conn(){
    return this->_free_conn;
}

void sql_pool::destroy_pool(){
    _mutex.mutex_lock();

    if(_conn_list.size() > 0){
        for(std::list<MYSQL*>::iterator it = _conn_list.begin(); it != _conn_list.end(); it++){
            MYSQL *conn = *it;
            mysql_close(conn);
        }

        _used_conn = 0;
        _free_conn = 0;
        _conn_list.clear();
    }

    _mutex.mutex_unlock();
}
