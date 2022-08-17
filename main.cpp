#include "Config/Config.h"


int main(int argc, char* argv[]){
    //数据库信息设定
    std::string user = "root";
    std::string passwd = "root";
    std::string dbname = "mydb";

    //初始化参数
    config config;
    config.parse_arg(argc, argv);

    //初始化webserver连接,设置详细细节
    WebServer server;

    server.init(config.port, user, passwd, dbname, config.log_write, config.sql_num, config.thread_num, config.close_log);
    
    //1、日志（同步、异步）
    server.log_write();
    //2、初始化数据库池
    server.SQL_pool();
    //std::cout<<"mysql information set successful"<<std::endl;

    //3、初始化线程池
    server.ThreadPool();
    //4、设定触发模式
    server.trig_mode();
    //5、初始化连接
    server.init_socket();
    std::cout<<"init success"<<std::endl;

    //6、epoll事件循环
    server.epoll_ev();

    //

    return 0;
}