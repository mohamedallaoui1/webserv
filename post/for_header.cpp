#include "../request.hpp"
#include "../post.hpp"
#include "../request.hpp"
#include "../multplixing.hpp"

std::map<int, std::vector<server*>::iterator> server_history;
std::map<int, int> client_history;
std::map<int, Client>  fd_maps; 

int isIP(std::string host) {
    int dots = 0;
    int doubleDots = 0;
    for (size_t i = 0; i < host.length(); i++) {
        if (host[i] == '.')
            dots++;
        if (host[i] == ':')
            doubleDots++;
    }
    if (dots == 3 && doubleDots == 1)
        return 1;
    if (host.substr(0, host.find_first_of(':')) == "localhost")
        return 1;
    return 0;
}

void request::getServer(int fd) {
    // magic hh
    int client_fd = client_history[fd];
    it = server_history[client_fd];
}

void checkifservername(std::string& ip, int& is_servername) {
    int count_dots = 0;
    for (size_t i = 0; i < ip.length(); i++) {
        if (ip[i] == '.')
            count_dots++;
    }
    if (count_dots != 3)
        is_servername = 1;
}

int request::parseHost(std::string hst, int fd) {
    std::string ip;
    std::string port;
    std::string incoming_port;
    std::string incoming_ip;
    std::map<int, Client>::iterator it3 = fd_maps.find(fd);
    getServer(fd);
    incoming_port = (*it)->cont["listen"];
    incoming_ip = (*it)->cont["host"];
    int is_servername = 0;
    // print with vold green the value of hst
    std::cout << "\033[1;38;5;82mSERVER NAME IS: '" << hst << "'\033[0m" << std::endl;
    ip = hst.substr(0, hst.find(':'));
    checkifservername(ip, is_servername);
    port = hst.substr(hst.find(':') + 1);
    if (port == "" || hst.find_first_of(':') == std::string::npos) {
        // print with wave red "port is empty"
    std::cout << "\033[5m\033[38;5;208mPORT IS EMPTY: '" << port << "'\033[0m" << std::endl;
        it3->second.resp.response_error("400", fd);
    }
    std::vector<server *>::iterator it2;
    for (it2 = fd_maps[fd].serv_.s.begin(); it2 != fd_maps[fd].serv_.s.end(); it2++) {
        if (!is_servername)
            break;
        if ((*it2)->cont["listen"] == incoming_port && (*it2)->cont["server_name"] == hst) {
            it = it2;
            return (0);
        }
    }
    for (it2 = fd_maps[fd].serv_.s.begin(); it2 != fd_maps[fd].serv_.s.end(); it2++) {
        if ((*it2)->cont["listen"] == incoming_port && (*it2)->cont["host"] == incoming_ip) {
            it = it2;
            return (0);
        }
        else
            continue;
    }
    return (1);
}

void request::parse_heade(std::string buffer, server &serv, int fd)
{
    std::istringstream stream (buffer);
    std::string line;
    getline(stream, line);
    std::vector<std::string> vec = serv.isolate_str(line , ' ');
    method = vec[0];
    path   = vec[1];
    while (getline(stream, line))
    {
        if (line.find("\r") != std::string::npos)
            line.erase(line.find("\r"));
        if (line.substr(0, 14) == "Content-Length")
            content_length = line.substr(16);
        else if (line.substr(0, 12) == "Content-Type")
            content_type = line.substr(14);
        else if (line.substr(0, 17) == "Transfer-Encoding")
            transfer_encoding = line.substr(19);
        else if (line.substr(0, 4) == "Host")
            parseHost(line.substr(6), fd);
        if (line == "\r")
            return ;
    }
}

void post::parse_header(std::string buffer, std::string &content_type, std::string &content_length, std::string &transfer_encoding)
{
    int g = 0;
    std::istringstream stream (buffer);
    std::string line;
    while (getline(stream, line))
    {
        if (line.find("\r") != std::string::npos)
            line.erase(line.find("\r"));
        if (line.substr(0, 14) == "Content-Length")
            content_length = line.substr(16);
        else if (line.substr(0, 12) == "Content-Type" && g == 0)
        {
            content_type = line.substr(14);
            // std::cout << "ssssssssssssssss: " << content_type << std::endl;
            g = 1;
        }
        else if (line.substr(0, 17) == "Transfer-Encoding")
            transfer_encoding = line.substr(19);
        if (line == "\r")
            return ;
    }
}