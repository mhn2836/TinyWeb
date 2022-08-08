#include "WebServer.h"

WebServer::WebServer(){
    //http连接数组初始化
    _users = new http_conn[MAX_FD];

    int _listenfd;
    char server_path[100];
    getcwd(server_path, sizeof(server_path));
    char *root = "/root";
    _root = (char *)malloc(strlen(server_path) + strlen(root) + 1);
    strcpy(_root, server_path);
    strcat(_root, root);
    
    //timer数组初始化
    _user_timer = new data[MAX_FD];
}

WebServer::~WebServer(){
    close(_efd);
    close(_listenfd);
    close(_pipe[1]);
    close(_pipe[0]);

    delete[] _users;
    delete[] _user_timer;
    delete _pool;
}

void WebServer::init(int port, std::string user, std::string passwd, std::string dbname, int log_write, int sql_num, int thread_num, int close_log){
    _port = port;
    _user = user;
    _password = passwd;
    _dbname = dbname;
    _log_write = log_write;
    _OPT_LINGER = 1;
    _trig_mode = 1;

    _sql_num = sql_num;
    _thread_num = thread_num;
    _close_log = close_log;
    _actor_model = 0;
}

void WebServer::log_write(){
    if(!_close_log){
        if(_log_write == 1)
            Log::get_instance()->init("./ServerLog", _close_log, 2000, 800000, 800); 
        else
            Log::get_instance()->init("./ServerLog", _close_log, 2000, 800000, 0); 
    }
}

void WebServer::trig_mode(){
    //ET模式    
    _listen_mode = 1;
    _conn_mode = 1;
}

void WebServer::sql_pool(){
    //
    _sql_pool = sql_pool::get_instance();
    _sql_pool -> init("localhost", _user, _passwd, _dbname, 3306, _sql_num, _close_log);

    users -> initmysql_result(_sql_pool);
}

void WebServer::thread_pool(){
    //
    _pool = new thread_pool<http_conn>(_thread_num, 10000, _sql_pool);
}

void WebServer::init_socket(){
    //socket - bind - listen
    _listenfd = socket(PF_INET, SOCK_STREAM, 0);
    std::assert(_listenfd >= 0);

    if(!_OPT_LINGER){
        struct linger temp = {0, 1};
        setsockopt(_listenfd, SOL_SOCKET, SO_LINGER, &temp, sizeof(temp));
    }
    else {
        struct linger temp = {1, 1};
        setsockopt(_listenfd, SOL_SOCKET, SO_LINGER, &temp, sizeof(temp));
    }

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(_port);

    int flag = 1;
    setsockopt(_listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
    ret = bind(_listenfd, (struct sockaddr *)&address, sizeof(address));
    assert(ret >= 0);
    ret = listen(_listenfd, 5);
    assert(ret >= 0);

    _utils.init(TIMESLOT);

    //epoll注册内核事件表
    epoll_event _events[MAX_EVENT_NUM];
    _efd = epoll_create(5);
    std::assert(_efd != -1);

    _utils.add_fd(_efd, _listenfd, false, _listen_mode);
    http_conn::m_epollfd = _efd;

    ret = socketpair(PF_UNIX, SOCK_STREAM, 0, _pipe);
    std::assert(ret != -1);

    _utils.set_non_blocking(_pipe[1]);
    _utils.add_fd(_efd, _pipe[0], false, 0);

    _utils.add_sig(SIGPIPE, SIG_IGN);
    _utils.add_sig(SIGALRM, _utils.sig_handler, false);
    _utils.add_sig(SIGTERM, _utils.sig_handler, false);

    alarm(TIMESLOT);

    util::_pipefd = _pipe;
    util::_efd = _efd;
}

void WebServer::timer(int cfd, struct sockaddr_in client_addr){
    util::_efd = _efd;
    _user_timer[cfd].init(cfd, client_addr, _root, _conn_mode, _close_log, 
        _user, _passwd, _dbname);

    _user_timer[cfd].addr = client_addr;
    _user_timer[cfd].sockfd = cfd;

    util_timer *timer = new util_timer(TIMESLOT);
    timer->_data = &_user_timer[cfd];
    timer->cb_func = cb_func;
    _user_timer[cfd].timer = timer;
    _utils._heap_timer.add_timer(timer);
}