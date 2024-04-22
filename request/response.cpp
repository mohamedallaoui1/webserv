#include "../request.hpp"
#include "../Client.hpp"

extern std::map<int, Client> fd_maps;

response::response()
{
        response_message = message_response_stat(); //
        std::cout << "resp constructor map size = " <<  response_message.size() << "\n";
}

response::~response(){

    // response_message.clear();
}

int     response::response_error(std::string stat, int fd)
{
         std::cout << "\t------------------- response_error ---------------\t"<< std::endl;
        std::string response;
        std::stringstream size;
        std::map<int, Client>::iterator it = fd_maps.find(fd);
        std::map<std::string, std::string>::iterator it_ = it->second.serv_.err_page.find(stat);
        
        std::cout << "map size = " <<  response_message.size() << "\n";
        std::cout << "state = " << stat << "\n";

        if( it_ != it->second.serv_.err_page.end())
        {
            std::cout << "stat " <<  it_->first << " Path " << it_->second << "\n";
            std::cout << "Stat  File Found=  " << stat << "\n";
            std::fstream    err_file;
            err_file.open(it_->second.c_str());
            char            buff_[1024];
            bzero(buff_, 1024);
            err_file.read(buff_, 1024).gcount();
            response = buff_;
            size << response.size();
            response = get_header(stat, "text/html", size.str(), it->second);
            response += it->second.get.to_string(buff_);
            std::cout << "response -> " << response << " <-\n";
            send(fd, response.c_str(), response.size(), 0);
            it->second.rd_done = 1;
            return (1);
        }
        else
        {          
            std::map<std::string, std::string>::iterator it_message_error = response_message.find(stat);
            if (it_message_error == response_message.end())
                return (0);
            std::cout << "here's map size = " <<  response_message.size() << "\n";
            std::string _respond_stat;
            _respond_stat = "<h1>" + it_message_error->second + "</h1>";
            _respond_stat += "<html><head><title> " + it_message_error->second + "</title></head>";
            _respond_stat +=  "<body> <strong>" + it_message_error->second + " </strong></body></html>";
            size << _respond_stat.size();
            std::cout << "resp ======> " << _respond_stat << "\n";
            response = get_header(stat, "text/html", size.str(), it->second);
            response += _respond_stat;
            std::cout << "delete responde -> " << response << "\n";
            send(fd, response.c_str(), response.size(), 0);
            it->second.rd_done = 1;
            return (1);
        }
        return (0);
}

std::map<std::string, std::string>        response::message_response_stat(/*std::map<std::string, std::string> &response_message*/)
{
    response_message["200"] = "OK";
    // response_message["201"] = "Created";
    // response_message["202"] = "Accepted";
    response_message["204"] = "No Content";
    response_message["301"] = "Moved Permanently";
    // response_message["302"] = "Found";
    // response_message["304"] = "Not Modified";
    response_message["400"] = "Bad Request";
    // response_message["401"] = "Unauthorized";
    response_message["403"] = "Forbidden";
    response_message["404"] = "Not Found";
    response_message["405"] = "Method Not Allowed";
    response_message["409"] = "Conflict";
    // response_message["500"] = "Internal Server Error";
    // response_message["505"] = "Version Not Supported";
    // response_message["501"] = "Not Implemented";
    // response_message["502"] = "Bad Gateway";
    // response_message["503"] = "Service Unavailable";
    // response_message["504"] = "Gateway Timeout";
    return (response_message);
}

std::string      response::get_header(std::string wich, std::string exten, std::string lentg, Client& fd_inf)
{
    std::string response;
    std::map<std::string , std::string>::iterator it = response_message.find(wich);
    if (it != response_message.end())
    {
        if (!wich.compare("200") || !wich.compare("404") || !wich.compare("403") 
        || !wich.compare("405") || !wich.compare("204") || !wich.compare("415") || !wich.compare("409"))
        {
            response = "HTTP/1.1 ";
            response +=  it->first + " " + it->second + "\r\n";
            response += "Content-Type: " + exten + "\r\n" + "Content-Length: " + lentg + "\r\n\r\n";
            fd_inf.res_header = 1;
            return (response);
        }
        else if (wich == "301")
        {
            std::string     path_with_slash = fd_inf.requst.path + "/";
            std::cout << " 3011 path <<=== " << path_with_slash << "\n";
            response = "HTTP/1.1 301 Moved Permanently\r\n";
            response += "Location: " + path_with_slash + "\r\n\r\n";
            fd_inf.res_header = 1;
            return (response);
        }
    }
    return "";
}
