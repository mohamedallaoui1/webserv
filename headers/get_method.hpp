#ifndef GET_METHOD_HPP
#define GET_METHOD_HPP

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
#include "request.hpp"
#include <sys/epoll.h>

class Client;

class get_method
{
    public:
        bool                checki;
        std::map<std::string, std::string> response_message;

        /*************** GET FUNCTIONS ***************/
        std::string     get_header(std::string wich, std::string exten, std::string lentg, Client&  fd_inf);
        std::string     generat_html_list(std::string directory);
        std::string     get_index_file(std::map<std::string, std::string> &loca_map);
        bool            check_autoindex(std::map<std::string, std::string> loca_map);
        int             get_mthod(int fd);
        int             check_exist(const std::string& path);
        std::streampos  get_fileLenth(std::string path);
        std::string     get_exten_type(std::string path, std::map<std::string, std::string> &exta);
        int             response_error(std::string stat, int fd);          
        std::map<std::string, std::string>             message_response_stat(/*std::map<std::string, std::string> &response_message*/);
        get_method();

        /********************************************/  
        template <typename T>
        std::string to_string(T value) 
        {
            std::ostringstream oss;
            oss << value;
            return oss.str();
        }
};

#endif
