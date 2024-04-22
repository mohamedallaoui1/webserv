#include "multplixing.hpp"

#define MAX_CLIENTS 

extern std::map<int, Client>  fd_maps; 
extern std::map<int, std::vector<server*>::iterator> server_history;
extern std::map<int, int> client_history;
int flag = 0;


in_addr_t multplixing::convertIpv4toBinary(const std::string& ip) {
    unsigned int parts[4];
    if (sscanf(ip.c_str(), "%u.%u.%u.%u", &parts[0], &parts[1], &parts[2], &parts[3]) != 4) {
        std::cerr << "Invalid IP address" << std::endl;
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 4; i++) {
        if (parts[i] > 255) {
            std::cerr << "Invalid IP address" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    uint32_t addr = (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3];
    return htonl(addr);
}



int        multplixing::close_fd(int fd)
{
    std::cout << "Client " << fd << " Was Removed From Map\n";
    std::cout << "it is Done\n";

    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd , NULL);
    close(fd);
    fd_maps.erase(fd_maps.find(fd));
    // exit(120);
    return 1;
}

int     multplixing::string_to_int(std::string str)
{
    return (atoi(str.c_str()));////////////////
}

void        multplixing::lanch_server(server parse)
{
    int bytesRead;
    int respo;
    request     rq;
    response     resp_;
    std::vector<server*>::iterator it;
    time_t  end;
    cgi cgi_;
    int is_cgi;

    epoll_fd = epoll_create(5);

    for (it = parse.s.begin(); it != parse.s.end(); it++)
    {
        int sockfd;
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("Socket creation failed");
            exit(EXIT_FAILURE);
        }
        serverSocket.push_back(sockfd);
        server_history[sockfd] = it;

        sockaddr_in sock_info;

        sock_info.sin_family = AF_INET;
        sock_info.sin_port = htons(string_to_int((*it)->cont["listen"]));
        uint32_t ip = convertIpv4toBinary((*it)->cont["host"]);
        sock_info.sin_addr.s_addr = ip;
        std::cout << "Ip Address : " << inet_ntoa(sock_info.sin_addr) << std::endl;
        int sp = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, &sp, sizeof(sp));
        if (bind(sockfd, (struct sockaddr *)&sock_info, sizeof(sock_info))) {
            perror("Bind failed");
            exit(EXIT_FAILURE);
        }
        if (listen(sockfd, 5)) {
            perror("Listen failed");
            exit(EXIT_FAILURE);
        }

        int flag;
    
        if ((flag = fcntl(sockfd, F_GETFL, 0)) < 0) {
            perror("Failed getting file status");
            exit(EXIT_FAILURE);
        }
        flag |= O_NONBLOCK;
        if (fcntl(sockfd, F_SETFL, flag) < 0) {
            perror("Failed setting file status");
            exit(EXIT_FAILURE);
        }

        
        struct epoll_event envts;
        envts.data.fd = sockfd;
        envts.events = EPOLLIN;
    
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &envts);

        std::cout << "Server is listening on port '" << (*it)->cont["listen"] << "'...\n";
    }

    while (true) 
    {
        signal(SIGPIPE, SIG_IGN); // magic this line ignore sigpip when you write to close fd the program exit by sigpip sign
        std::string buffer;
        std::vector<int>::iterator it;

        signal(SIGPIPE, SIG_IGN); // magic this line ignore sigpip when you write to close fd the program exit by sigpip sign
        std::cout << "whyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy\n";
        int num = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < num; i++) {
            is_cgi = 0;
            if ((it = std::find(serverSocket.begin(), serverSocket.end(), events[i].data.fd)) != serverSocket.end()) {
                std::cout << "BEFORE CLIENT FD VALUE :" << events[i].data.fd << std::endl;
                std::cout << "New Client Connected\n";
                int client_socket = accept(*it, NULL, NULL);
                struct epoll_event envts_client;
                envts_client.data.fd = client_socket;
                envts_client.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLRDHUP | EPOLLHUP ;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &envts_client) == -1)
                {
                    close(client_socket);
                    perror("Issue In Adding Client To Epoll");
                    exit(1);
                }
                fd_maps[client_socket] = Client();
                fd_maps[client_socket].serv_      = parse;
                client_history[client_socket]     = *it;
                fd_maps[client_socket].cgi_       = cgi_;
                fd_maps[client_socket].start_time = time(NULL);
                std::cout << "AFTER CLIENT FD VALUE :" << events[i].data.fd << std::endl;
                std::cout << "Client " << client_socket << " Added To Map\n";
            }
            else {
                std::map<int, Client>::iterator it_fd = fd_maps.find(events[i].data.fd);
                std::cout << "Client with an event :" << events[i].data.fd << std::endl;
                if (events[i].events & EPOLLRDHUP || events[i].events & EPOLLERR  || events[i].events & EPOLLHUP) 
                {
                    if (close_fd( events[i].data.fd ))
                        continue ;
                }
                else if (events[i].events & EPOLLIN)
                {
                    std::cout << "FD READY TO READ -_- = " << events[i].data.fd << " \n";
                    buffer.resize(BUFFER_SIZE);
                    bytesRead = recv(events[i].data.fd , &buffer[0], BUFFER_SIZE, 0);
                    std::cout << "\n\n\t -> bytesRead ==== " << bytesRead << std::endl;
                    if (bytesRead > 0)
                        buffer.resize(bytesRead);
                    if (bytesRead <= 0)
                    {
                       if (close_fd( events[i].data.fd ))
                            continue ;
                    }
                    std::cout << buffer << "\n";

                    if (flag == 0)
                    {
                        if (buffer.find("\r\n\r\n") != std::string::npos)
                        {
                            rq.parse_req(buffer, parse, events[i].data.fd );
                            fd_maps[events[i].data.fd].requst     = rq;
                            fd_maps[events[i].data.fd].resp       = resp_;
                            // fd_maps[events[i].data.fd].cgi        = cgi_;
                            std::cout << " stat = " << it_fd->second.not_allow_method << "\n";
                            if (it_fd->second.not_allow_method)
                            {
                                it_fd->second.not_allow_method      = 0;
                                //print with vold red "I AM DEAD"
                                std::cout << "\033[1;31mI AM DEAD\033[0m" << std::endl;
                                if (close_fd(events[i].data.fd))
                                    continue ;
                            }
                            // flag = 1;
                        }
                    }
                    if (!it_fd->second.requst.redirect_path.empty())
                    {
                        std::cout << "redirect ==> " << it_fd->second.requst.redirect_path << "\n";
                        std::cout << "redirect_path ==> " << it_fd->second.requst.path << "\n";
                        if (it_fd->second.resp.response_error("301", events[i].data.fd))
                        {
                            std::cout << "redirect ==> " << it_fd->second.requst.redirect_path << "\n";
                            std::cout << "redirect_path ==> " << it_fd->second.requst.path << "\n";
                             it_fd->second.requst.redirect_path.clear();
                             it_fd->second.requst.path.clear();
                             std::cout << "<--- fd ---> " << events[i].data.fd  << "\n";
                            if (close_fd(events[i].data.fd))
                            {
                                continue ;
                            }
                        }
                    }
                    if (rq.method == "POST" && flag == 1 && !it_fd->second.not_allow_method)
                    {
                        fd_maps[events[i].data.fd].post_.j = 0;
                        if (fd_maps[events[i].data.fd].post_.post_method(buffer)  && !it_fd->second.not_allow_method)
                            fd_maps[events[i].data.fd].post_.j = 1;
                    }
                    fd_maps[events[i].data.fd].u_can_send = 1;
                    std::cout << "CGI TESTING : '" << fd_maps[events[i].data.fd].requst.stat_cgi << "'" << std::endl;
                    fd_maps[events[i].data.fd].cgi_.checkifcgi(rq, is_cgi, events[i].data.fd);
                    if (!fd_maps[events[i].data.fd].requst.stat_cgi.compare("on")) {
                        if (is_cgi)
                            fd_maps[events[i].data.fd].cgi_.cgi_method(rq, events[i].data.fd);
                    }
                }
                else if (events[i].events & EPOLLOUT && !it_fd->second.rd_done && it_fd->second.u_can_send) // must not always enter to here i think ask about it 
                {
                    // flag = 0;
                    std::cout << "ready  writing " << " \n";
                    respo = 0;
                    if (!fd_maps[events[i].data.fd].requst.method.compare("GET"))
                        respo = (*it_fd).second.get.get_mthod(events[i].data.fd);
                    if (!fd_maps[events[i].data.fd].requst.method.compare("DELETE"))
                    {
                        std:: string res_delete = (*it_fd).second.delet.delet_method((*it_fd).second.requst.uri, (*it_fd).second.serv_, events[i].data.fd);
                        if (!res_delete.compare("delete_stat"))
                        {
                             if (close_fd( events[i].data.fd ))
                                continue ;
                        }
                        else if (!res_delete.compare("delete"))
                        {
                            if (it_fd->second.resp.response_error("204", events[i].data.fd))
                            {
                                if (close_fd( events[i].data.fd ))
                                    continue ;
                            }
                        }
                    }
                    if (!fd_maps[events[i].data.fd].requst.method.compare("POST") && fd_maps[events[i].data.fd].post_.j)
                    {
                        std::string response = "HTTP/1.1 201 OK\r\nContent-Type: text/html\r\n\r\nhello";
                        send(events[i].data.fd,response.c_str(), response.length(), 0);
                        respo = 1;
                    }
                    std::cout << "\t\t stat kaml wla ba9i == "      << it_fd->second.rd_done << std::endl;
                    std::cout << "\t\t second.not_allow_method == " << it_fd->second.not_allow_method<< std::endl;
                    if (respo || it_fd->second.not_allow_method)
                    {
                        it_fd->second.not_allow_method = 0;
                        std::cout << "\t\t SF KAML GHADI UTM7A HAD "  << events[i].data.fd << std::endl;
                        if (close_fd( events[i].data.fd ))
                            continue ;
                    }
                }
                else {
                    end = time(NULL);
                    if (difftime(end, fd_maps[events[i].data.fd].start_time) > MAX_TIME) {
                        fd_maps[events[i].data.fd].resp.response_error("408", events[i].data.fd);
                        close_fd(events[i].data.fd);
                        
                        continue;
                    }
                }
            }
        }
    }
}


