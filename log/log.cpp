#include "log.h"


log::log():_lines_count(0), _is_async(false){}

log::~log(){if(_fp) fclose(_fp);}

void* log::async_write_log(){
    std::string single_log;
    while(_log_queue -> pop(single_log)){
        _mutex.mutex_lock();
        fputs(single_log.c_str(), _fp);
        _mutex.mutex_unlock();
    }
}

bool log::init(const char* filename, int close_log, int log_buffer = 8192,
    int split_lines = 5000000, int max_queue = 0){
    //设置队列长度时，默认异步日志模式。
    if(max_queue >= 1){
        _is_async = true;
        _log_queue = new block_queue<std::string>(max_queue);
        pthread_t tid;
        pthread_create(&tid, NULL, flush_log, NULL);
    }

    _close_log = close_log;
    _log_buffer = log_buffer;
    _buffer = new char[_log_buffer];
    memset(_buffer, '\0', _log_buffer);
    _split_lines = split_lines;

    time_t t = time(NULL);
    struct tm *sys_t = localtime(&t);
    struct tm my_tm = *sys_t;

    const char *p = strrchr(filename, '/');
    char log_full_name[256] = {0};

    //初始化的时候日志写入日期信息
    if(!p) snprintf(log_full_name, 255, "%s%d_%02d_%02d_%s", my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday, filename);

    else {
        strcpy(_log,p + 1);
        strncpy(_dir, filename, p - filename + 1);
        snprintf(log_full_name, 255, "%s%d_%02d_%02d_%s", _dir, my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday, _log);
    }

    _day = my_tm.tm_mday;
    _fp = fopen(log_full_name, "a");

    return _fp != NULL;

}

void log::write_log(int level, const char *format, ...){
    struct timeval now = {0, 0};
    gettimeofday(&now, NULL);
    time_t t = now.tv_sec;
    struct tm *sys_tm = localtime(&t);
    struct tm my_tm = *sys_tm;

    char s[16] = {0};
    //根据模式写入不同的字符串
    switch (level)
    {
    case 1:
        strcpy(s, "[debug]:");
        break;
    case 2:
        strcpy(s, "[info]:");
        break;
    case 3:
        strcpy(s, "[warn]:");
        break;
    case 4:
        strcpy(s, "[erro]:");
        break;
    default:
        strcpy(s, "[info]:");
        break;
    }

    _mutex.mutex_lock();
    _lines_count++;

    if(_day != my_tm.tm_mday || _lines_count % _split_lines == 0){
        char new_log[256] = {0};
        fflush(_fp);
        fclose(_fp);

        char tail[16] = {0};
        snprintf(tail, 16, "%d_%02d_%02d_", my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday);

        if(_day != my_tm.tm_mday){
            snprintf(new_log, 255, "%s%s%s", _dir, tail, _log);
            _day = my_tm.tm_mday;
            _lines_count = 0;
        }
        else {
            snprintf(new_log, 255, "%s%s%s.%lld", _dir, tail, _log, _lines_count / _split_lines);
        }
        _fp = fopen(new_log, "a");
    }

    _mutex.mutex_unlock();

    va_list valst;
    va_start(valst, format);
    std::string log_str;

    _mutex.mutex_lock();
    //写入各项信息
    int n = snprintf(_buffer, 48, "%d-%02d-%02d %02d:%02d:%02d.%06ld %s ",
                     my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday,
                     my_tm.tm_hour, my_tm.tm_min, my_tm.tm_sec, now.tv_usec, s);
    
    int m = vsnprintf(_buffer + n, _log_buffer - 1, format, valst);
    _buffer[n + m] = '\n';
    _buffer[n + m + 1] = '\0';
    log_str = _buffer;

    _mutex.mutex_unlock();

    if(_is_async && !_log_queue -> full()){
        _log_queue -> push(log_str);
    }
    else {
        _mutex.mutex_lock();
        fputs(log_str.c_str(), _fp);
        _mutex.mutex_unlock();
    }

    va_end(valst);

}

void log::flush(){
    _mutex.mutex_lock();
    fflush(_fp);
    _mutex.mutex_unlock();
}