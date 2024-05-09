#include "../headers/request.hpp"
#include "../headers/Client.hpp"
#define MAX_PATH = 1000;
extern std::map<int, Client *> fd_maps;
extern int query;


int               request::one_of_allowed(std::string mehod, std::vector<std::string> allowed_methods)
{
    if (allowed_methods.empty())
    {
        k = 1;
        return 0;
    }
    std::vector<std::string>::iterator it = allowed_methods.begin();
    std::vector<std::string>::iterator ite = allowed_methods.end();
    if (!(*it).compare("allow_methods"))
        it++;
    while (it != ite)
    {
        if (!it->compare(mehod))
            return 1;
        it++;   
    }
    return 0;
}

int            request::check_cgi_exten(std::string exten)
{
    std::map<std::string, std::string>::iterator it_bi = cgi_map.begin();
    std::map<std::string, std::string>::iterator it_end = cgi_map.end();
    while (it_bi != it_end)
    {
        if (!it_bi->first.compare(exten))
        {
            x_cgi = 1;
            return 1;
        }
        it_bi++;
    }
    return 0;
}

int            request::check_path_access(std::string path)
{
    char current_path_[1000];
    std::string compare_root_loca = loca__root.substr(0, loca__root.length() - 1);
    if (realpath(path.c_str(), current_path_) !=   NULL)
    {
        std::string current_path(current_path_);
        if (current_path.compare(0, compare_root_loca.length(), compare_root_loca) != 0)
            return (1);
    }
    return (0);
}

int    checkcgi(request& rq, int& iscgi, int fd) {
    std::string query;
    if (rq.uri.find("?") != std::string::npos) {
        query = rq.uri.substr(rq.uri.find("?"));
        rq.uri = rq.uri.substr(0, rq.uri.find("?"));
        query = query.substr(query.find_last_of('?') + 1);
        fd_maps[fd]->cgi_.QUERY_STRING = query;
    }
    std::string path = rq.uri;
    std::string::iterator it = path.begin() + path.find_last_of("/") + 1;
    if (it == path.end()) {
        iscgi = 0;
        return 0;
    }
    std::string file = std::string(it, path.end());
    if (fd_maps[fd]->requst.cgi_map.find(file.substr(file.find_last_of(".") + 1)) != fd_maps[fd]->requst.cgi_map.end()) {
        iscgi = 1;
        return 1;
    }
    return 0;
}

char hex_to_char(const std::string& hex) {
    std::istringstream iss(hex);
    int value;
    iss >> std::hex >> value;
    return static_cast<char>(value);
}

std::string hex_to_ascii(const std::string& input) {
    std::string result;
    for (size_t i = 0; i < input.length(); ++i) {
        if (input[i] == '%') {
            std::string hex_str = input.substr(i + 1, 2);
            char hex_char = hex_to_char(hex_str);
            result += hex_char;
            i += 2;
        } else {
            result += input[i];
        }
    }
    return result;
}

int            request::parse_req(std::string   rq, server &server, int fd) // you can remove the server argenent
{
    if (parse_heade(rq, server, fd) == 1)
        return 1;
    std::cout << "severNAME : " << (*it)->cont["server_name"] << std::endl;
    if (!fd_maps[fd]->err) {
        fd_maps[fd]->err_page = (*it)->err_page;
        fd_maps[fd]->err = 1;
    }
    std::map<int, Client *>::iterator it = fd_maps.find(fd);
    int             state;
    it->second->resp.response_message = server.response_message;

    last          = rq.find("\r\n");
    vec           = server.isolate_str(rq.substr(0, last) , ' ');
    if (vec.size() != 3 || last == std::string::npos)
    {
        state = it->second->resp.response_error("400", fd);    
        it->second->not_allow_method = 1;
        return 0;        
    }
    method        = vec[0];
    path          = vec[1];
    http_version  = vec[2];
    if (http_version.compare("HTTP/1.1"))
    {
        state = it->second->resp.response_error("505", fd);    
        it->second->not_allow_method = 1;
        return 0;
    } 
    if (it->second->serv_.check_forbidden(path))
    {
        state = it->second->resp.response_error("400", fd);    
        it->second->not_allow_method = 1;
        return 0;        
    }
    if (path == "/favicon.ico")
    {
        state = it->second->resp.response_error("404", fd);
        it->second->not_allow_method = 1;
        return 0;
    }
    uri = get_full_uri(server, *it->second);
    if (uri.empty())
    {
        state = it->second->resp.response_error("404", fd);    
        it->second->not_allow_method = 1;
        return 0;
    }
    checkcgi(*this, fd_maps[fd]->is_cgi, fd);
    if (access(uri.c_str(), F_OK) < 0)
        uri = hex_to_ascii(uri);
    x = it->second->get.check_exist(uri);
    if (redirection_stat == 1)
    {
        std::string msg = "HTTP/1.1 301 Moved Permanently\r\nlocation: " + it->second->redirec_path + "\r\n\r\n";
        write(fd, msg.c_str(), msg.length());
        it->second->not_allow_method = 1;
        return 0;
    }
    if (check_path_access(uri))
    {
        state = it->second->resp.response_error("403", fd);    
        it->second->not_allow_method = 1;
        return 0;        
    }
    if ((method.compare("DELETE") && method.compare("POST") && method.compare("GET")) || !method_state)
    {
        if ((method.compare("DELETE") && method.compare("POST") && method.compare("GET")))
        {
            state = it->second->resp.response_error("501", fd);
            it->second->not_allow_method = 1;
            return 0;
        }
        if (!method_state && !k){
            state = it->second->resp.response_error("405", fd);
            it->second->not_allow_method = 1;
            return 0;
        }
    }
    reset();
    return 0;
}

std::string     request::find_longest_path(server &server, Client &obj)
{
    (void)obj;
    (void)server;
    std::string long_path;
    std::string path__;
    check_redi = 0;
    path__ = path;
    if (path[path.length() - 1] != '/')
        path__ = path + "/";
    std::vector<std::string>::iterator it_b  = (*it)->vec_of_locations.begin(); 
    std::vector<std::string>::iterator ite = (*it)->vec_of_locations.end();
    for(; it_b != ite; it_b++)
    {
        if (path__.find(*it_b) == 0)
        {
            check_redi = 1; 
            if (it_b->length() > long_path.length()) 
                long_path = *it_b;
        }
    }
    if (check_redi && path[path.length() - 1] != '/' && path__.substr(long_path.length()).empty()) // check kayn location and request url mamsaliach b '/' and makin ta haja mnwra '/'  /v/index.html
    {
        long_path = "move_permently";
        return (long_path);
    }
    return (long_path);
}

std::string     request::get_full_uri(server &server, Client& obj)
{
    int     loca_found = 0;
    longest_loca = find_longest_path(server, obj);
    // if (longest_loca == "/" && path.length() > 1)
    //     return ("404");
    if (longest_loca == "move_permently")
    {
        obj.redirec_path = path + "/";
        redirection_stat = 1;
        return("move_permently");
    }
    redirection_stat = 0;
    if (path.length() > longest_loca.length())
        rest_fldr   = path.substr(longest_loca.length()); 
    for (size_t j = 0; j < (*it)->l.size(); j++)
    {
        loca_found = rewrite_location((*it)->l[j]->cont_l);
        if (loca_found)
        {
            cgi_map = (*it)->l[j]->cgi_map;
            if (one_of_allowed(method, (*it)->l[j]->allowed_methods))
                method_state = true;
            if (!(*it)->l[j]->redirction_path.empty() && redirection_stat == 0)
            {
                obj.redirec_path = (*it)->l[j]->redirction_path;
                redirection_stat = 1;
            }
            break ;
        }
    }
    return full_path;
}

int           request::rewrite_location(std::map<std::string, std::string> location_map)
{
    std::map<std::string, std::string>::iterator      ite = location_map.end();
    std::map<std::string, std::string>::iterator      it_cgi_check = location_map.find("cgi_status");

    if (it_cgi_check == location_map.end())
        stat_cgi = "off";
    for (std::map<std::string, std::string>::iterator itb = location_map.begin(); itb != ite; itb++)
    {
        if ((!(*itb).first.compare("upload")))
            upload_state = (*itb).second;
        if ((!(*itb).first.compare("root")))
            loca__root = (*itb).second;
        if ((!itb->first.compare("cgi_status")))  
            stat_cgi = itb->second;
        if ((!(*itb).first.compare("upload")))
            upload_state = (*itb).second;
        if ((!(*itb).first.compare("upload_path")))
        {
            upload_path = (*itb).second;
        }
    }
    for (std::map<std::string, std::string>::iterator itb = location_map.begin(); itb != location_map.end(); itb++)
    {
        if ((!(*itb).first.compare("location") && !(*itb).second.compare("/"))) // found bool is false in case location not found !
            root_map = location_map;
        if ((!(*itb).first.compare("location") &&  !itb->second.compare(longest_loca)))
        {
            found = true;
            auto_index_stat = check_autoindex(location_map);
            std::map<std::string, std::string>::iterator it_b = location_map.find("root");
            if (!rest_fldr.empty()) // rest 3amr
            {
                full_path = (*it_b).second + "/" + rest_fldr;
                check = 1;
                return 1;
            }
            else // add index on specify location
            {
                std::map<std::string, std::string>::iterator indx = location_map.find("index");
                if (indx != location_map.end())
                {
                    full_path = (*it_b).second + "/" + (*indx).second; 
                    check = 1;
                    return 1;
                }
                else
                {
                    full_path = (*it_b).second; 
                    check = 1;
                    return 1;
                }
            }
        }
    }
    return 0;
}  

bool            request::check_autoindex(std::map<std::string, std::string> loca_map)
{
    std::map<std::string, std::string>::iterator it_e = loca_map.end();
    for (std::map<std::string, std::string>::iterator it_b = loca_map.begin(); it_b != it_e; it_b++)
    {

        if (!(*it_b).first.compare("autoindex"))
        {
            if (!(*it_b).second.compare("on"))
                auto_index_stat = true;
            break;
        }
    }
    return (auto_index_stat);
}

void        request::fill_extentions()
{   
    extentions["html"] = "text/html; charset=UTF-8"; 
    extentions["txt"]  = "text/plain"; 
    extentions["jpg"] = "image/jpg"; 
    extentions["jpeg"] = "image/jpeg";
    extentions["png"] = "image/png";
    extentions["mp3"] = "audio/mpeg";
    extentions["mp4"] = "video/mp4";
    extentions["webm"] = "video/webm";
    extentions["pdf"] = "application/pdf";
    extentions["zip"] = "application/zip";
    extentions["woff"] = "application/font-woff";
    extentions["js"] = "application/javascript";
    extentions["css"] = "text/css";
    extentions["xml"] = "text/xml";
    extentions["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    extentions["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    extentions["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    extentions["svg"] = "image/svg+xml";
    extentions["json"] = "application/json";
    extentions["ico"] = "image/x-icon";
    extentions["gif"] = "image/gif";
    extentions["mpg"] = "video/mpeg";
    extentions["avi"] = "video/x-msvideo";
    extentions["mov"] = "video/quicktime";
    extentions["m3u8"] = "application/vnd.apple.mpegurl";
    extentions["wasm"] = "application/wasm";
    extentions["mpd"] = "application/dash+xml";
    extentions["db"] = "application/x-sqlite3";
    extentions["md"] = "text/markdown";
    extentions["py"] =  "text/html; charset=UTF-8";
}

void request::reset() 
{
    k = 0;
    method_state = false;
    http_version.clear();
    loca_fldr.clear();
    rest_fldr.clear();
    full_path.clear();
    full_rest.clear();
    redirect_path.clear();
}

std::string     request::get_exten_type(std::string path)
{
    std::string exten;
    size_t      pos = path.find_last_of(".");
    
    if (pos != std::string::npos)
        exten = path.substr(pos + 1);
    if (pos == std::string::npos && x == 1)
        return ("application/octet-stream");        
    std::map<std::string, std::string>::iterator b = extentions.find(exten);
    if (b != extentions.end())
        return ((*b).second);
    if ((b == extentions.end() ) && !check_cgi_exten(exten) && x == 1)
        return ("text/html");
    return "10";
}

std::streampos  request::get_fileLenth(std::string path)
{
    std::ifstream file(path.c_str(), std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return -1; // Return -1 to indicate error
    }
    file.seekg(0, std::ios::end);
    std::streampos file_Size = file.tellg();
    file.seekg(0, std::ios::beg);
    file.close();
    return file_Size;
}

request::request(/* args */){
    found = false;
    check = false;
    auto_index_stat = false;
    method_state = false;
    redirct_loca = 0;
    loca_fldr = "";
    longest_loca = "";
    rest_fldr = "";
    full_path = "";
    full_rest = "";
    redirect_path = "";
    x_cgi = 0;
    x = 0;
    k = 0;
    fill_extentions();
}
request::~request(){
    reset();
}