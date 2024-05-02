#include "../Client.hpp"
#include "../request.hpp"
#include "../get_method.hpp"

extern std::map<int, Client> fd_maps;

Client::Client(std::string uri_)
{
    stor_uri             = uri_;
    res_header           = 0;
    rd_done              = 0; 
    res_sent             = 0;
    u_can_send           = 0;
    version_not_suported = 0;
    file_not_supported   = 0;
    not_allow_method     = 0;
}

Client::Client(const Client& copy)
{
    this->u_can_send  = copy.u_can_send; 
    this->filePosition = copy.filePosition;
    this->rd_done = copy.rd_done;
    this->res_header = copy.res_header;
    this->stor_uri = copy.stor_uri;
    this->redirec_path = copy.redirec_path;
}

Client&         Client::operator=(const Client& copy)
{
    this->u_can_send  = copy.u_can_send; 
    this->filePosition = copy.filePosition;
    this->rd_done      = copy.rd_done;
    //  this->read_f    = copy.read_f;
    this->res_header   = copy.res_header;
    this->stor_uri     = copy.stor_uri;
    this->res_sent     = copy.res_sent;
    this->redirec_path     = copy.redirec_path;
    this->not_allow_method     = copy.not_allow_method;
    return (*this);
}

Client::~Client()
{
    // std::cout<<"================CLIENT dESTOR================\n";
    kill(cgi_.clientPid, 9);
    waitpid(cgi_.clientPid, NULL, 0);
    unlink(cgi_.file_out.c_str());
}

Client::Client()
{
    // std::cout<<"-----------------new Client-----------------\n";
    res_header          = 0;
    rd_done             = 0;
    u_can_send          = 0;
    res_sent            = 0;
    not_allow_method    = 0;
    redirec_path        = "";
}


std::map<std::string, std::string>            Client::message_response_stat()
{
        response_message["200"] = "OK";
        // response_message["201"] = "Created";
        // response_message["202"] = "Accepted";
        response_message["204"] = "No Content";
        response_message["301"] = "Moved Permanently";
        // response_message["302"] = "Found";
        // response_message["304"] = "Not Modified";
        // response_message["400"] = "Bad Request";
        // response_message["401"] = "Unauthorized";
        response_message["403"] = "Forbidden";
        response_message["404"] = "Not Found";
        response_message["405"] = "Method Not Allowed";
        response_message["415"] = "Unsupported Media Type";
        // response_message["501"] = "Not Implemented";
        // response_message["502"] = "Bad Gateway";
        // response_message["503"] = "Service Unavailable";
        response_message["504"] = "Gateway Timeout";
        response_message["505"] = "HTTP Version Not Supported";
        return response_message;
}
