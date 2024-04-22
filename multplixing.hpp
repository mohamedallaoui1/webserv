#ifndef MULTIPLIXING_HPP
#define MULTIPLIXING_HPP



#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <map>
#include <vector>
#include "Client.hpp"


# define MAX_EVENTS 1024
# define BUFFER_SIZE 1024
# define MAX_TIME 20

class multplixing
{
    private:
        struct epoll_event events[MAX_EVENTS];
    public:
        void    lanch_server(server parse);server              parse;
        int     epoll_fd;
        std::vector<int>   serverSocket;
        int     string_to_int(std::string str);
        in_addr_t convertIpv4toBinary(const std::string& ip);
        int     close_fd(int fd);
        multplixing(){
        };
};


#endif
