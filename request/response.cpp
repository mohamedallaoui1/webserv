#include "../headers/request.hpp"
#include "../headers/Client.hpp"

extern std::map<int, Client *> fd_maps;

response::response()
{
    response_message = message_response_stat(); //
}

response::~response(){
}

int     response::response_error(std::string stat, int fd)
{
        std::string response;
        std::stringstream size;
        std::map<std::string, std::string>::iterator it_ = fd_maps[fd]->err_page.find(stat);

        if( it_ != fd_maps[fd]->err_page.end())
        {
            std::ifstream    err_file;
            std::stringstream sstr;
            err_file.open(it_->second.c_str());
            sstr << err_file.rdbuf();
            response = sstr.str();
            size << response.size();
            response = get_header(stat, "image/png", size.str(), *fd_maps[fd]);
            response += sstr.str();
            usleep(200000);
            send(fd, response.c_str(), response.size(), 0);
            fd_maps[fd]->rd_done = 1;
            return (1);
        }
        else
        {          
            std::map<std::string, std::string>::iterator it_message_error = response_message.find(stat);
            if (it_message_error == response_message.end())
            {
                return (0);
            }
            std::string _respond_stat;
            _respond_stat = "<h1>" + it_message_error->second + "</h1>";
            _respond_stat += "<html><head><title> " + it_message_error->second + "</title></head>";
            _respond_stat +=  "<body> <strong>" + it_message_error->second + " </strong></body></html>";
            size << _respond_stat.size();
            response = get_header(stat, "text/html", size.str(), *fd_maps[fd]);
            response += _respond_stat;
            usleep(200000);
            send(fd, response.c_str(), response.size(), 0);
            fd_maps[fd]->rd_done = 1;
            return (1);
        }
        return (0);
}

std::map<std::string, std::string>        response::message_response_stat(/*std::map<std::string, std::string> &response_message*/)
{
    response_message["200"] = "OK";
    response_message["201"] = "Created";
    response_message["202"] = "Accepted";
    response_message["204"] = "No Content";
    response_message["301"] = "Moved Permanently";
    response_message["302"] = "Found";
    response_message["304"] = "Not Modified";
    response_message["400"] = "Bad Request";
    response_message["401"] = "Unauthorized";
    response_message["403"] = "Forbidden";
    response_message["404"] = "Not Found";
    response_message["405"] = "Method Not Allowed";
    response_message["409"] = "Conflict";
    response_message["413"] = "Request Entity Too Large";
    response_message["415"] = "Unsupported Media Type";
    response_message["500"] = "Internal Server Error";
    response_message["408"] = "Request Timeout";
    response_message["505"] = "Version Not Supported";
    response_message["501"] = "Not Implemented";
    response_message["502"] = "Bad Gateway";
    response_message["503"] = "Service Unavailable";
    response_message["504"] = "Gateway Timeout";
    return (response_message);
}

std::string      response::get_header(std::string wich, std::string exten, std::string lentg, Client& fd_inf)
{
    std::string response;
    std::map<std::string , std::string>::iterator it = response_message.find(wich);
    int a = std::atoi(wich.c_str());
    if (it != response_message.end())
    {
        if (a != 301 && (a >= 200 && a < 599))
        {
            response = "HTTP/1.1 ";
            response +=  it->first + " " + it->second + "\r\n";
            response += "Content-Type: " + exten + "\r\n" + "Content-Length: " + lentg + "\r\n\r\n";
            fd_inf.res_header = 1;
            return (response);
        }
        else if (a == 301)
        {
            if (fd_inf.requst.redirection_stat)
            {
                std::string     path_with_slash = fd_inf.redirec_path;
                response = "HTTP/1.1 301 Moved Permanently\r\n";
                response += "Location: " + path_with_slash + "\r\n\r\n";
                fd_inf.res_header = 1;
                return (response);
            }
            else
            {
                std::string     path_with_slash = fd_inf.requst.path + "/";
                response = "HTTP/1.1 301 Moved Permanently\r\n";
                response += "Location: " + path_with_slash + "\r\n\r\n";
                fd_inf.res_header = 1;
                return (response);
            }
        }
    }
    return "";
}