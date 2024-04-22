#ifndef REQUEST_HPP
#define REQUEST_HPP

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
#include <sys/epoll.h>


class Client;
class server;
class request
{
    public:
        std::string                         content_type;
        std::string                         content_length;
        std::string                         transfer_encoding;
        std::string                         uri;
        std::string                         path;
        std::string                         method;
        std::string                         http_version;
        std::string                         upload_state;
        std::string                         loca_fldr;
        std::string                         rest_fldr;
        std::string                         full_path;
        std::string                         full_rest;
        std::string                         loca__root;
        size_t                              pos;
        bool                                found;
        bool                                method_state;
        bool                                auto_index_stat;
        unsigned long                       g;
        size_t                              check;
        int                                 x;
        int                                 redirct_loca;
        int                                 x_cgi;
        std::string                         longest_loca;
        std::string                         stat_cgi;
        std::string                         line;
        size_t                              last;
        std::vector<std::string>            vec;
        std::string                         extention_type;
        std::string                         redirect_path;
        std::map<std::string, std::string>  extentions;
        std::map<std::string, std::string>  cgi_map;
        std::map<std::string, std::string>  root_map;
        std::map<std::string, std::string>  response_message;
        std::vector<server*>::iterator      it;

        /*************** REQUEST FUNCTIONS ***************/

        int                                       check_path_access(std::string path);
        int                                       check_cgi_exten(std::string exten);
        void                                      reset();
        void                                      check_cgi_exten(int fd);
        void                                      print_map(std::map<std::string, std::string> m);
        int                                       rewrite_location(std::map<std::string, std::string> location_map);
        void                                      fill_response_varbls(std::string &path);
        void                                      parse_heade(std::string buffer, server &serv, int fd);
        std::string                               get_delet_resp(std::string path, int stat);
        void                                      fill_extentions();
        std::streampos                            get_fileLenth(std::string path);
        std::string                               get_exten_type(std::string path);
        bool                                      check_autoindex(std::map<std::string, std::string> loca_map);


        std::string                                get_full_uri( server &server, Client &obj);
        std::string                                find_longest_path(server &server, Client &obj);
        std::string                                delet_method(std::string path, server &server);
        void                                       parse_req(std::string   rq, server &server, int fd);
        int                                        one_of_allowed(std::string mehod, std::vector<std::string> allowed_methods);
        int                                        parseHost(std::string hst, int fd);
        void                                       getServer(int fd);
        

        template <typename T>
        std::string to_string(T value) 
        {
            std::ostringstream oss;
            oss << value;
            return oss.str();
        }
        request();
        ~request();
};


#endif