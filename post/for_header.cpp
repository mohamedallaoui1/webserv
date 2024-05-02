#include "../request.hpp"
#include "../Client.hpp"
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
    ip = hst.substr(0, hst.find(':'));
    checkifservername(ip, is_servername);
    port = hst.substr(hst.find(':') + 1);
    if ((server::check_ip(ip) || server::valid_range(port)) && !is_servername)
        it3->second.resp.response_error("400", fd);
    if (port == "" || hst.find_first_of(':') == std::string::npos) {
        it3->second.resp.response_error("400", fd);
        multplixing::close_fd(fd, fd_maps[fd].epoll_fd);
        isfdclosed = true;
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

int request::parse_heade(std::string buffer, server &serv, int fd)
{
    std::istringstream stream (buffer);
    std::string line;
    getline(stream, line);
    std::vector<std::string> vec = serv.isolate_str(line , ' ');
    method = vec[0];
    path   = vec[1];
    if (buffer.find("Host") == std::string::npos) {
        if (fd_maps[fd].resp.response_error("400", fd)) {
            std::cout << "22222222222222222222\n";
            if (multplixing::close_fd(fd, fd_maps[fd].epoll_fd))
                return 1;
        }
    }
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
            return 0;
    }
    return 0;
}

void post::parse_header(std::string buffer)
{
    int t = 0;
    std::istringstream stream (buffer);
    std::string line;
    while (getline(stream, line))
    {
        if (line.find("\r") != std::string::npos)
            line.erase(line.find("\r"));
        if (line.substr(0, 14) == "Content-Length")
            content_length = line.substr(16);
        else if (line.substr(0, 12) == "Content-Type" && t == 0)
        {
            content_type = line.substr(14);
            t = 1;
        }
        else if (line.substr(0, 17) == "Transfer-Encoding")
        {
            transfer_encoding = line.substr(19);
            g = 10;
        }
        if (line == "\r")
            return ;
    }
}