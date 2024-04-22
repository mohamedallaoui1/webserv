#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <map>
#include <vector>
#include <fstream>
#include <sys/epoll.h>
#include "get_method.hpp"
#include "post.hpp"
#include "delete.hpp"
#include "response.hpp"
#include "cgi.hpp"
#include "time.h"

class delete_;
class response;

class Client
{
    public:
        std::map<std::string, std::string>  response_message;
        int                                 res_header;
        std::string                         stor_uri;
        std::ifstream                       read_f;
        int                                 rd_done;
        int                                 res_sent;
        int                                 u_can_send;
        int                                 not_allow_method;
        int                                 version_not_suported;
        int                                 file_not_supported;
        std::streampos                      filePosition;
        get_method                          get;
        request                             requst;
        response                            resp;
        post                                post_;
        server                              serv_;
        delete_                             delet;
        cgi                                 cgi_;
        time_t                              start_time;
        Client(std::string uri_);
        Client(const Client& copy);
        std::map<std::string, std::string>  message_response_stat();
        Client&                             operator=(const Client& copy);
        Client&                             operator=(std::string _uri);
        Client();
        ~Client();
};

#endif
