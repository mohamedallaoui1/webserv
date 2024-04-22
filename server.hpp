#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <cstdlib>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>
#include <vector>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <sys/stat.h>
#include <dirent.h>
#include "location.hpp"

#include <netinet/in.h>
#include <arpa/inet.h>


class server : public location
{
    public:
        std::string                         server_n;
        std::string                         port;
        std::string                         id;
        std::string                         listen;
        std::string                         s_root;
        std::string                         _root;
        std::string                         v_index;
        std::vector<server*>                s;
        std::vector<location*>              l;
        std::map<std::string, std::string>  cont;
        std::map<std::string, std::string>  cont_prev;
        location                            obj;
        std::map<std::string, std::string>  err_page;
        std::string                         str;
        std::string                         str_l;
        std::vector<std::string>            str_l_vec;
        int                                 s_token;
        std::vector<std::string>            s_vec;
        std::string                         check;
        std::map<int, int>                  req_time;
        std::vector<std::string>            indexs;
        std::map<std::string, std::string>  response_message;
        std::vector<std::string>            vec_of_locations;
        std::vector<std::string>            p_s_id;

        std::string                         controle_slash(std::string direc);
        void                                check_duplicate_location(std::vector<std::string> s);
        void                                check_server_deplicate();
        void                                message_response_stat();
        bool                                isWhitespace(std::string str); 
        int                                 check_stat(std::string &stat_error);
        void                                mange_file(const char* file);
        void                                handle_err(const std::string  file);
        std::string                         strtrim(std::string &str);
        void                                print_err(std::string str);
        std::vector<std::string>            isolate_str(std::string s, char deli);
        void                                stor_values(std::vector<std::string> s, char ch);
        void                                free_l(std::vector<location*> &lv);
        int                                 parse_loca(std::ifstream& rd_cont, std::string &str_);
        int                                 parse_both(std::ifstream& rd_cont, std::string &str_);
        void                                check_size(std::vector<std::string> &s, char c);
        void                                handl_serv(std::vector<std::string> s);
        int                                 is_num(std::string s);
        int                                 valid_range(std::string s);
        int                                 check_permi(std::string path);
        int                                 check_exist(std::string path);
        int                                 check_ip_nbr(std::string nbr);
        int                                 check_ip(std::string ip);
        server(std::map<std::string, std::string> &cont_s, std::vector<location*> &l_, std::vector<std::string> &vec_of_locations_);   
        server();


};

#endif
