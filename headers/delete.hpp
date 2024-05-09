#ifndef DELETE_HPP
#define DELETE_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <map>
#include <vector>
#include <fstream>
#include "server.hpp"
#include "Client.hpp"
#include <sys/epoll.h>

class server;

class delete_
{
    public:
        std::string     delet_method(std::string path, server &server, int fd);
        delete_();
        ~delete_();
};

#endif