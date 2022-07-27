#ifndef HTTP_CONN_H
#define HTTP_CONN_H

#include<string>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<netinet/in.h>
#include<errno.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<cassert>

#include "http_request.h"
#include "http_response.h"

class http_conn{
public:
    http_conn();
    ~http_conn();
private:

};


#endif