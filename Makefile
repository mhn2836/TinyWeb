CXX ?= g++

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g
else
    CXXFLAGS += -O2

endif

server: main.cpp  ./timer/timer.cpp ./http_conn/http_conn.cpp ./log/log.cpp ./thread_pool/sql_pool.cpp ./WebServer/WebServer.cpp ./Config/Config.cpp
	$(CXX) -o server  $^ $(CXXFLAGS) -lpthread -lmysqlclient

clean:
	rm  -r server