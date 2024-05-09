#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <string>
#include <sys/time.h>
#include <cstring>
#include <cstdlib>
#include <sys/wait.h>
#include <iostream>
#include <cstring>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include "server.hpp"
#include "request.hpp"

class Client;

class response
{
    public:
        std::map<std::string, std::string> response_message;
        int         response_error(std::string stat, int fd);
        std::string get_header(std::string wich, std::string exten, std::string lentg, Client& fd_inf);
        std::map<std::string, std::string>        message_response_stat();
        response();
        ~response();
};

#endif