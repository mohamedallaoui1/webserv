#include "../headers/request.hpp"
#include "../headers/Client.hpp"
#include "../headers/multplixing.hpp"

std::map<int, std::vector<server*>::iterator> server_history;
std::map<int, int> client_history;
std::map<int, Client *>  fd_maps; 

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
    std::map<int, Client *>::iterator it3 = fd_maps.find(fd);
    getServer(fd);
    incoming_port = (*it)->cont["listen"];
    incoming_ip = (*it)->cont["host"];
    int is_servername = 0;
    std::string::size_type n = std::count(hst.begin(), hst.end(), ':');
    ip = hst.substr(0, hst.find(':'));
    std::cout << "ip : " << ip << std::endl;
    checkifservername(ip, is_servername);
    port = hst.substr(hst.find(':') + 1);
    if (((server::check_ip(ip) || server::valid_range(port)) && !is_servername) || n != 1) {
        it3->second->resp.response_error("400", fd);
        multplixing::close_fd(fd, fd_maps[fd]->epoll_fd);
        isfdclosed = true;
        return 1;
    }
    if (port == "" || hst.find_first_of(':') == std::string::npos) {
        it3->second->resp.response_error("400", fd);
        multplixing::close_fd(fd, fd_maps[fd]->epoll_fd);
        isfdclosed = true;
        return 1;
    }
    std::vector<server *>::iterator it2;
    for (it2 = fd_maps[fd]->serv_.s.begin(); it2 != fd_maps[fd]->serv_.s.end(); it2++) {
        if (!is_servername)
            break;
        if ((*it2)->cont["listen"] == incoming_port && (*it2)->cont["server_name"] == ip) {
            it = it2;
            return (0);
        }
    }
    for (it2 = fd_maps[fd]->serv_.s.begin(); it2 != fd_maps[fd]->serv_.s.end(); it2++) {
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
    if (buffer.find("Host:") == std::string::npos) {
        if (fd_maps[fd]->resp.response_error("400", fd)) {
            if (multplixing::close_fd(fd, fd_maps[fd]->epoll_fd))
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
        else if (line.substr(0, 4) == "Host") {
            if (parseHost(line.substr(6), fd))
                return 1;
        }
        else if (line.substr(0, 6) == "Cookie")
            fd_maps[fd]->cgi_.HTTP_COOKIE = line.substr(8);
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
    content_length = "";
    content_type = "";
    transfer_encoding = "";
    while (getline(stream, line))
    {
        if (line.find("Content-Length:") != std::string::npos)
        {
            content_length = line.substr(16);
            content_length.erase(content_length.find("\r"));
        }
        else if (line.find("Content-Type:") != std::string::npos && t == 0)
        {
            content_type = line.substr(14);
            content_type.erase(content_type.find("\r"));
            t = 1;
        }
        else if (line.find("Transfer-Encoding:") != std::string::npos)
        {
            transfer_encoding = line.substr(19);
            transfer_encoding.erase(transfer_encoding.find("\r"));
            g = 10;
        }
    }
}