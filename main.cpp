#include "Config/Config.h"


int main(int argc, char* argv[]){
    //数据库信息设定
    std::string user = "root";
    std::string passwd = "root";
    std::string dbname = "mydb";

    //std::cout<<"mysql information set successful"<<std::endl;

    //初始化设定
    config config;
    config.parse_arg(argc, argv);

    //初始化webserver连接,设置详细细节
    WebServer server;

    server.init(config.port, user, passwd, dbname, config.log_write, config.sql_num, config.thread_num, config.close_log);
    std::cout<<"successful"<<std::endl;
    //1、日志（同步、异步）
    server.log_write();

    server.SQL_pool();
    server.ThreadPool();
    server.trig_mode();

    
    
    server.init_socket();
    server.epoll_ev();

    //

    return 0;
}