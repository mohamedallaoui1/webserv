#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <map>
#include <vector>
#include <fstream>
#include <fcntl.h>


class cgi
{
    public:
        int                                 idx;
        int                                 stat_cgi;
        void                                cgi_extention();
        void                                cgi_work(int fd);
        std::vector<std::string>            cgi_env;
        // std::map<std::string, std::string>  cgi_env;
        char**                              env;
        std::map<std::string,std::string>   exten_cgi;
        void                                get_exten_type(int fd);
        void                                fill_env_cgi(Client &obj);
        void                                checkifcgi(request& rq, int& iscgi, int fd);
        void                                cgi_method(request& rq, int fd);
        char **                             fillCgiEnv(int fd);
        std::string                         cgi_stat;
        std::string                         compiler;
        std::string                         file_out;
        std::string                         file_err;
        std::string                         file_in;
        pid_t                               clientPid;
        int                                 is_error;

        // Envirement //
        std::string                         REQUEST_METHOD;
        std::string                         SCRIPT_NAME;
        std::string                         QUERY_STRING;
        std::string                         CONTENT_TYPE;
        std::string                         CONTENT_LENGTH;
        std::string                         SCRIPT_FILENAME;
        std::string                         REDIRECT_STATUS;
        std::string                         SERVER_PORT;

        // ---------- //
        
        cgi();
        cgi& operator=(const cgi& copy) {
            this->idx = copy.idx;
            this->stat_cgi = copy.stat_cgi;
            this->cgi_env = copy.cgi_env;
            this->env = copy.env;
            // this->exten_cgi = copy.exten_cgi;
            this->cgi_stat = copy.cgi_stat;
            this->REQUEST_METHOD = copy.REQUEST_METHOD;
            this->SCRIPT_NAME = copy.SCRIPT_NAME;
            this->QUERY_STRING = copy.QUERY_STRING;
            this->CONTENT_TYPE = copy.CONTENT_TYPE;
            this->CONTENT_LENGTH = copy.CONTENT_LENGTH;
            this->SCRIPT_FILENAME = copy.SCRIPT_FILENAME;
            this->REDIRECT_STATUS = copy.REDIRECT_STATUS;
            this->SERVER_PORT = copy.SERVER_PORT;

            return *this;
        }
        ~cgi();
};

#endif
