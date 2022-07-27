#include "WebServer.h"

WebServer::WebServer(int port, std::string user, std::string passwd, std::string dbname, 
        int log_write,int opt_linger, int trigger_mode, int sql_num, int thread_num, 
        int close_log, int actor_model):
        _port(port), _logwrite(log_write), _optlinger(opt_linger), 
        _triggermode(trigger_mode), _sqlnum(sql_num), _threadnum(thread_num), _closelog(close_log),
        _actormodel(actor_model){

        char server_path[100];
        getcwd(server_path, sizeof(server_path));
        char *root = "/root";
        _root = (char *)malloc(strlen(server_path) + strlen(root) + 1);
        strcpy(_root, server_path);
        strcat(_root, root);

        users = new http_conn;

}

WebServer::~WebServer(){

}

void WebServer::log_write(){

}

void WebServer::init_socket(){

}

void WebServer::trig_mode(){

}

void WebServer::sql_pool(){

}

void WebServer::thread_pool(){

}