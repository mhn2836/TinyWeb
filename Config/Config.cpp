#include "Config.h"

config::config():port(9006), log_write(0), trig_mode(1), conn_mode(0), opt_linger(0),
    sql_num(8), thread_num(8), close_log(0){}

config::~config(){}

void config::parse_arg(int argc, char* argv[]){
    int opt;
    const char *str = "p:l:m:o:s:t:c:";
    while ((opt = getopt(argc, argv, str)) != -1)
    {
        switch (opt)
        {
        case 'p':
        {
            port = atoi(optarg);
            break;
        }
        case 'l':
        {
            log_write = atoi(optarg);
            break;
        }
        case 'm':
        {
            trig_mode = atoi(optarg);
            break;
        }
        case 'o':
        {
            opt_linger = atoi(optarg);
            break;
        }
        case 's':
        {
            sql_num = atoi(optarg);
            break;
        }
        case 't':
        {
            thread_num = atoi(optarg);
            break;
        }
        case 'c':
        {
            close_log = atoi(optarg);
            break;
        }
        default:
            break;
        }
    }
}