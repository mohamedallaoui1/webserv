#include "multplixing.hpp"

#define MAX_CLIENTS 

extern std::map<int, Client *>  fd_maps; 
extern std::map<int, std::vector<server*>::iterator> server_history;
extern std::map<int, int> client_history;
std::vector<void *> garbage;
int isfdclosed;
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

int        multplixing::close_fd(int fd, int epll)
{
    //"Client " << fd << " Was Removed From Map\n";
    //"it is Done\n";

    epoll_ctl(epll, EPOLL_CTL_DEL, fd , NULL);
    fd_maps.erase(fd_maps.find(fd));
    //"THE VALUE OF FD:" << fd << std::endl;
    close(fd);
    delete fd_maps[fd];
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
    request     *rq = new request();
    response     resp_;
    std::vector<server*>::iterator it;
    time_t  end;
    cgi cgi_;
    
    bool check_cgi = false;

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
        //"Ip Address : " << inet_ntoa(sock_info.sin_addr) << std::endl;
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

        //"Server is listening on port '" << (*it)->cont["listen"] << "'...\n";
    }

    while (true) 
    {
        signal(SIGPIPE, SIG_IGN); // magic this line ignore sigpip when you write to close fd the program exit by sigpip sign
        std::string buffer;
        std::vector<int>::iterator it;

        signal(SIGPIPE, SIG_IGN); // magic this line ignore sigpip when you write to close fd the program exit by sigpip sign
        //"whyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy\n";
        int num = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < num; i++) {
            check_cgi = false;
            isfdclosed = false;
            if ((it = std::find(serverSocket.begin(), serverSocket.end(), events[i].data.fd)) != serverSocket.end()) {
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
                fd_maps[client_socket]             = new Client();
                fd_maps[client_socket]->serv_      = parse;
                client_history[client_socket]      = *it;
                fd_maps[client_socket]->cgi_       = cgi_;
                fd_maps[client_socket]->epoll_fd   = epoll_fd;
                fd_maps[client_socket]->is_cgi     = 0;
                fd_maps[client_socket]->start_time = time(NULL);
                fd_maps[client_socket]->flagg      = 0;
                // fd_maps[client_socket]->istimeout  = false;
            }
            else {
                end = time(NULL);
                std::map<int, Client *>::iterator it_fd = fd_maps.find(events[i].data.fd);
                if (events[i].events & EPOLLRDHUP || events[i].events & EPOLLERR  || events[i].events & EPOLLHUP) 
                {
                    if (close_fd( events[i].data.fd, epoll_fd ))
                        continue ;
                }
                if ((difftime(end, fd_maps[events[i].data.fd]->start_time) > MAX_TIME) /*&& fd_maps[events[i].data.fd]->istimeout */) {
                    if (fd_maps[events[i].data.fd]->resp.response_error("408", events[i].data.fd)) {
                        if (close_fd(events[i].data.fd, epoll_fd))
                            continue ;
                    }
                }
                else if (events[i].events & EPOLLIN)
                {
                    fd_maps[events[i].data.fd]->cgi_.stat_cgi = 0;
                    buffer.resize(BUFFER_SIZE);
                    bytesRead = recv(events[i].data.fd , &buffer[0], BUFFER_SIZE, 0);
                    if (!fd_maps[events[i].data.fd]->flagg)
                    {
                        fd_maps[events[i].data.fd]->buf += buffer;
                        fd_maps[events[i].data.fd]->join_readbytes += bytesRead;
                    }
                    else
                    {
                        fd_maps[events[i].data.fd]->buf = buffer;
                        fd_maps[events[i].data.fd]->join_readbytes = bytesRead;
                    }
                    if (bytesRead > 0)
                        fd_maps[events[i].data.fd]->buf.resize(fd_maps[events[i].data.fd]->join_readbytes);
                    if (bytesRead == -1 || bytesRead == 0)
                    {
                        if (close_fd( events[i].data.fd, epoll_fd ))
                            continue ;
                    }
                    if (bytesRead > 1)
                        fd_maps[events[i].data.fd]->start_time = time(NULL);

                    if (!fd_maps[events[i].data.fd]->flagg)
                    {
                        if (fd_maps[events[i].data.fd]->buf.find("\r\n\r\n") != std::string::npos)
                        {
                            std::cout  << " IN THE REQUEST PARISNG\n";
                            fd_maps[events[i].data.fd]->flagg = 1;
                            if (rq->parse_req(fd_maps[events[i].data.fd]->buf, parse, events[i].data.fd ) == 1) {
                                        continue ;
                            }
                            // std::cout << "hereeeee\n";
                            if (isfdclosed)
                                continue;
                            fd_maps[events[i].data.fd]->requst     = rq;
                            garbage.push_back(rq);
                            fd_maps[events[i].data.fd]->resp       = resp_;
                            if (it_fd->second->not_allow_method)
                            {
                                it_fd->second->not_allow_method      = 0;
                                if (close_fd(events[i].data.fd, epoll_fd))
                                    continue ;
                            }
                        }
                        else if (fd_maps[events[i].data.fd]->buf.find("\r\n\r\n") == std::string::npos && !fd_maps[events[i].data.fd]->flagg)
                        {
                            std::cout << "not in the request parsing.\n";
                            continue;
                        }
                        else {
                            time_t end1 = time(NULL);
                            if ((difftime(end1, fd_maps[events[i].data.fd]->start_time) > MAX_TIME)) {
                                if (fd_maps[events[i].data.fd]->resp.response_error("408", events[i].data.fd)) {
                                    if (close_fd(events[i].data.fd, epoll_fd))
                                        continue ;
                                }
                            }
                        }
                    }
                    if (!fd_maps[events[i].data.fd]->requst->stat_cgi.compare("on"))
                        fd_maps[events[i].data.fd]->cgi_.checkifcgi(*rq, fd_maps[events[i].data.fd]->is_cgi, events[i].data.fd);
                    else if (!fd_maps[events[i].data.fd]->requst->stat_cgi.compare("off")){
                        if (fd_maps[events[i].data.fd]->resp.response_error("403", events[i].data.fd)) {
                            if (close_fd(events[i].data.fd, epoll_fd))
                                continue ;
                        }
                    }
                    /**************** FOR POST METHOD *********************/
                    fd_maps[events[i].data.fd]->post_.g = 0;
                    fd_maps[events[i].data.fd]->post_.j = 0;
                    std::cout << "max_body = '" << (*fd_maps[events[i].data.fd]->requst->it)->cont["server_name"] << "'\n";
                    if (rq->method == "POST" && fd_maps[events[i].data.fd]->flagg == 1 && !it_fd->second->not_allow_method)
                    {
                        if (fd_maps[events[i].data.fd]->is_cgi)
                        {
                            // std::cout << fd_maps[events[i].data.fd]->requst->upload_path << "-> uploade in post -----------\n";
                            if (fd_maps[events[i].data.fd]->post_.post_method(fd_maps[events[i].data.fd]->buf, events[i].data.fd)  && !it_fd->second->not_allow_method)
                            {
                                fd_maps[events[i].data.fd]->post_.j = 1;
                                fd_maps[events[i].data.fd]->flagg = 0;
                            }
                        }
                        if (fd_maps[events[i].data.fd]->requst->upload_state != "on" && !fd_maps[events[i].data.fd]->is_cgi)
                        {
                            if (it_fd->second->resp.response_error("403", events[i].data.fd))
                            {
                                fd_maps[events[i].data.fd]->post_.g = 0;
                                if (close_fd(events[i].data.fd, epoll_fd))
                                    continue ;
                            }
                        }
                        if (fd_maps[events[i].data.fd]->post_.post_method(fd_maps[events[i].data.fd]->buf, events[i].data.fd)  && !it_fd->second->not_allow_method && !fd_maps[events[i].data.fd]->is_cgi)
                        {
                            fd_maps[events[i].data.fd]->post_.j = 1;
                            fd_maps[events[i].data.fd]->flagg = 0;
                        }
                        if (fd_maps[events[i].data.fd]->post_.g == 1)
                        {
                            //"bad request.\n";
                            if (it_fd->second->resp.response_error("400", events[i].data.fd))
                            {
                                // std::cout << "why?\n";
                                fd_maps[events[i].data.fd]->post_.g = 0;
                                if (close_fd(events[i].data.fd, epoll_fd))
                                    continue ;
                            }
                        }
                        else if (fd_maps[events[i].data.fd]->post_.g == 2) // unsupported media type;
                        {
                            if (it_fd->second->resp.response_error("415", events[i].data.fd))
                            {
                                fd_maps[events[i].data.fd]->post_.g = 0;
                                if (close_fd(events[i].data.fd, epoll_fd))
                                    continue ;
                            }
                        }
                        else if (fd_maps[events[i].data.fd]->post_.g == 3)
                        {
                            //"413 error message\n";
                            if (it_fd->second->resp.response_error("413", events[i].data.fd))
                            {
                                fd_maps[events[i].data.fd]->post_.g = 0;
                                if (close_fd(events[i].data.fd, epoll_fd))
                                    continue ;
                            }
                        }
                        else if (fd_maps[events[i].data.fd]->post_.g == 4) // Not implemented.
                        {
                            if (it_fd->second->resp.response_error("501", events[i].data.fd))
                            {
                                fd_maps[events[i].data.fd]->post_.g = 0;
                                if (close_fd(events[i].data.fd, epoll_fd))
                                    continue ;
                            }
                        }
                    }
                    /****************        end        *********************/
                    fd_maps[events[i].data.fd]->u_can_send = 1;
                    //"CGI TESTING : '" << fd_maps[events[i].data.fd]->requst->stat_cgi << "'" << std::endl;
                    if (fd_maps[events[i].data.fd]->is_cgi && !check_cgi) {
                        
                        fd_maps[events[i].data.fd]->cgi_.cgi_method(*rq, events[i].data.fd);
                        check_cgi = true;
                    }
                }
                else if (events[i].events & EPOLLOUT && !it_fd->second->rd_done && it_fd->second->u_can_send) // must not always enter to here i think ask about it 
                {
                    // fd_maps[events[i].data.fd]->istimeout = true;
                    // std::cout << "client " << events[i].data.fd << " is ready to send" << " \n";
                    respo = 0;
                    if (!fd_maps[events[i].data.fd]->requst->method.compare("GET")) {
                        respo = (*it_fd).second->get.get_mthod(events[i].data.fd);
                    }
                    if (isfdclosed)
                    {
                        continue;
                    }
                    if (!fd_maps[events[i].data.fd]->requst->method.compare("DELETE"))
                    {
                        std:: string res_delete = (*it_fd).second->delet.delet_method((*it_fd).second->requst->uri, (*it_fd).second->serv_, events[i].data.fd);
                        if (!res_delete.compare("delete_stat"))
                        {
                             if (close_fd( events[i].data.fd, epoll_fd ))
                                continue ;
                        }
                        else if (!res_delete.compare("delete"))
                        {
                            if (it_fd->second->resp.response_error("204", events[i].data.fd))
                            {
                                if (close_fd( events[i].data.fd, epoll_fd ))
                                    continue ;
                            }
                        }
                    }
                    if (!fd_maps[events[i].data.fd]->requst->method.compare("POST") && fd_maps[events[i].data.fd]->post_.j)
                    {
                        if (it_fd->second->resp.response_error("201", events[i].data.fd))
                        {
                            if (close_fd( events[i].data.fd, epoll_fd ))
                                continue ;
                        }
                        respo = 1;
                    }
                    // std::cout << "\t\t stat kaml wla ba9i == "      << it_fd->second->rd_done << std::endl;
                    // std::cout <<"\t\t second.not_allow_method == " << it_fd->second->not_allow_method<< std::endl;
                    if (respo || it_fd->second->not_allow_method)
                    {
                        it_fd->second->not_allow_method = 0;
                        // std::cout << "\t\t SF KAML GHADI UTM7A HAD "  << events[i].data.fd << std::endl;
                        if (close_fd( events[i].data.fd, epoll_fd ))
                            continue ;
                    }
                }
                
            }
        }
    }
}