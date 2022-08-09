#ifndef CONFIG_H
#define CONFIG_H

#include "../WebServer/WebServer.h"

class config{
public:
    config();
    ~config();

    void parse_arg(int argc, char* argv[]);

    int port;
    int log_write;
    int trig_mode;
    int listen_mode;
    int conn_mode;
    int opt_linger;
    int sql_num;
    int thread_num;
    int close_log;
};



#endif