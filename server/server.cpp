#include "../server.hpp"
#include "../multplixing.hpp"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"

int i = 0;
extern std::vector<void *> garbage;

server::server()
{
    max_body = "2147483647";
    duplicate["index"]          = 0;
    duplicate["root"]           = 0;
    duplicate["allow_methods"]  = 0;
    duplicate["autoindex"]      = 0;
    duplicate["cgi_status"]     = 0;
    duplicate["upload_path"]    = 0;
    duplicate["upload"]         = 0;
    message_response_stat();
}

server::server(std::map<std::string, std::string> &cont_s, std::vector<location*> &l_, std::vector<std::string> &vec_of_locations_)
{
    cont = cont_s;
    l = l_;
    vec_of_locations = vec_of_locations_;
}

std::string     server::strtrim(std::string &str)
{
    size_t  start;
    size_t  end;
    
    start = str.find_first_not_of(" ");
    if (start != std::string::npos)
        str = str.substr(start);
    end = str.find_last_not_of(" ");
    if (end != std::string::npos)
        str = str.substr(0, end + 1);
    return (str);
}

void            server::print_err(std::string str)
{
    std::cout << str << std::endl;
    exit(1);
}

std::vector<std::string>    server::isolate_str(std::string s, char deli)
{
    std::vector<std::string> vec;
    std::string              words;
    
    strtrim(s);
    std::istringstream       stream(s);
    while (std::getline(stream, words, deli))
    {
        if (!words.empty())
            vec.push_back(words);
    }
    return (vec);
}

void    server::check_server_deplicate()
{
    for (size_t i = 0; i < s.size(); i++)
    {
        if (s[i]->cont.find("server_name") == s[i]->cont.end())
            print_err("SERVER NAME IT'S NOT FOUND");
        for(size_t j = i + 1; j < s.size(); j++)
        {
            if (s[i]->cont["listen"] == s[j]->cont["listen"] && s[i]->cont["host"] == s[j]->cont["host"] 
                && s[i]->cont["server_name"] == s[j]->cont["server_name"])
            {
                print_err ("Duplicate Servers");
                return ;
            }
        }
    }
}

void        server::check_duplicate_location(std::vector<std::string> s)
{
    for (size_t i = 0; i < s.size(); i++)
    {
        for (size_t j = i + 1; j < s.size(); j++)
        {
            if (s[i] == s[j])
                print_err ("Duplicate Location");
        }
    }
}

void        server::mange_file(const char* file)
{
    std::ifstream   rd_content(file);
    s_token = 0;
    obj.l_token = 0;

    while (std::getline(rd_content, str)) // loop to get lines
    {
        str = strtrim(str);
        if (str.empty() || isWhitespace(str) || str[0] == '#')
            continue;
        if (str.compare("server"))
            print_err("syntaxt_error server");
        std::getline(rd_content, str); // store all servers
        str = strtrim(str);
        if (!str.compare("{"))
        {
            s_token++;
            int g = parse_both(rd_content, str);
            // parse_both(rd_content,str);
            if (/*(!str.compare("}") && s_token == 1)*/ (g == 2 && s_token == 2))
            {
                check_duplicate_location(vec_of_locations);
                server *new_s = new server(cont, l, vec_of_locations);
                s.push_back(new_s);
                garbage.push_back(new_s);
                cont.clear();
                l.clear();
                vec_of_locations.clear();
                s_token = 0;
            }
        }
    }
    check_server_deplicate();
}

void           server::message_response_stat()
{
        response_message["200"] = "OK";
        // response_message["201"] = "Created";
        // response_message["202"] = "Accepted";
        response_message["204"] = "No Content";
        response_message["301"] = "Moved Permanently";
        response_message["408"] = "Request Timeout";
        // response_message["302"] = "Found";
        // response_message["304"] = "Not Modified";
        response_message["400"] = "Bad Request";
        // response_message["401"] = "Unauthorized";
        response_message["403"] = "Forbidden";
        response_message["404"] = "Not Found";
        response_message["405"] = "Method Not Allowed";
        response_message["415"] = "Unsupported Media Type";
        response_message["501"] = "Not Implemented";
        // response_message["502"] = "Bad Gateway";
        // response_message["503"] = "Service Unavailable";
        response_message["504"] = "Gateway Timeout";
        response_message["505"] = "HTTP Version Not Supported";
        response_message["500"] = "Internal Server Error";
        return ;
}

int        server::parse_loca(std::ifstream& rd_cont, std::string &str_)
{
    std::vector<std::string>    v_s;
    while (std::getline(rd_cont, str)) // loop to iterate inside location
    {
        str_ = strtrim(str_);
        if (str_.empty() || isWhitespace(str_) || str[0] == '#')
            continue ;
        l_vec = isolate_str(str_, ' ');
        if (!l_vec[0].compare("root") || !l_vec[0].compare("index") 
            || !l_vec[0].compare("limit_except") || !l_vec[0].compare("allow_methods") 
            || !l_vec[0].compare("autoindex") || !l_vec[0].compare("upload") 
            || !l_vec[0].compare("upload_path") || !l_vec[0].compare("cgi_path") 
            || !l_vec[0].compare("cgi_status") || !l_vec[0].compare("return"))
            {
                check_size(l_vec, 'l'); 
                if (!l_vec[0].compare("allow_methods"))
                    v_s = l_vec;
                if (!l_vec[0].compare("root"))
                    _root = l_vec[1];
                if (!l_vec[0].compare("cgi_path"))
                    cgi_map[l_vec[1]] = l_vec[2];
                if (!l_vec[0].compare("return"))
                    redirction_path = l_vec[1];
                stor_values(l_vec, 'l');
            }
        else if (!l_vec[0].compare("}"))
        {
            obj.l_token++;
            if (obj.l_token == 2)
            {
                // make map that store path location and root , you have root_
                handl_loca(cont_l, v_s, _root);
                cgi_extention();
                location *new_l = new location(cont_l, v_s, cgi_map, redirction_path);
                l.push_back(new_l);
                garbage.push_back(new_l);
                check_dup();
                cont_l.clear();
                redirction_path.clear();
                v_s.clear();
                // cgi_map.clear();
                break ;
            }
        }
        else
            print_err("syntaxt_error unkown directive");
    }
    if (obj.l_token == 2)
    {
        obj.l_token = 0;
        while (std::getline(rd_cont, str_l))
        {
            str_l = strtrim(str_l);
            str_l_vec = isolate_str(str_l, ' '); // }
            if (isWhitespace(str_l) || str_l.empty()) // modify
                continue;
            if (!str_l_vec[0].compare("location"))
            {
                check_size(str_l_vec, 'l');
                vec_of_locations.push_back(controle_slash(str_l_vec[1]));
                cont_l[str_l_vec[0]]    = controle_slash(str_l_vec[1]); // store location with its path
                return 1 ;
            }
            if (!str_l_vec[0].compare("}"))
            {
                s_token++;
                return 0;
            }
        }
        if (rd_cont.eof() && s_token == 1)
            print_err ("syntaxt_error }");
    }
    return 1;
}

int     server::is_num(std::string s)
{
    for (size_t i = 0; i < s.size(); i++)
    {
        if (!std::isdigit(s[i]))
            return 1;
    }
    return 0;
}

int     server::valid_range(std::string s)
{
    int num;
    num = std::atoi(s.c_str());

    if (num < 1 || num > 65535)
        return 1;
    return 0;
}

int     server::check_exist(std::string path)
{
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) == 0) 
    {
        if (S_ISREG(fileStat.st_mode) || S_ISDIR(fileStat.st_mode))
        {
            return 1; // Path exists and is a regular file
        }
    }
    return 0; // Path does not exist
}

bool server::isWhitespace(std::string str) 
{
    for (size_t i = 0; i < str.length(); ++i) {
        if (!std::isspace(str[i])) {
            return false; 
        }
    }
    return true;
}

int    server::parse_both(std::ifstream& rd_cont, std::string &str_)
{
    while (std::getline(rd_cont, str_)) // loop to iterate inside server
    {
        str_ = strtrim(str_);
        if (str_.empty() || isWhitespace(str_) || str[0] == '#')
            continue ;
        s_vec = isolate_str(str_, ' ');
        if (s_token == 1 && !s_vec[0].compare("}"))
            break;
        else if (s_vec[0].compare("listen")&& s_vec[0].compare("error_page") 
        && s_vec[0].compare("client_max_body_size") && s_vec[0].compare("host") 
        && s_vec[0].compare("server_name"))
        {
            if (s_vec[0].compare("location") && s_vec[0].compare("{"))
                print_err("syntaxt_error");    
            else if (!s_vec[0].compare("location") || !s_vec[0].compare("{"))
            {
                if (s_vec[0].compare("{")) // you mean that is not { then it is a location
                {
                    check_size(s_vec, 'l'); // check first loca's path
                    vec_of_locations.push_back(controle_slash(s_vec[1]));
                    std::cout << "first location after normal = " << controle_slash(s_vec[1]) << "\n";
                    cont_l[s_vec[0]] = controle_slash(s_vec[1]);
                    std::getline(rd_cont, str_);
                    str = strtrim(str);
                }
                if (!str_.compare("{"))
                {
                    obj.l_token++;
                    int c = parse_loca(rd_cont, str_);
                    if (!c)
                        return 2; 
                    if (c == 1)
                        continue;
                }
            }
        }
        else 
        {
            check_size(s_vec, 's');
            if (!s_vec[0].compare("error_page"))
            {
                if (check_exist(s_vec[2]) && !check_stat(s_vec[1])) // check also stat lik 404 301 ...
                    err_page[s_vec[1]] = s_vec[2];
                else
                    print_err("syntaxt_error on the error_page");
            }
            handl_serv(s_vec); 
            stor_values(s_vec, 's');
        }
    }
    return (0);
}

int        server::check_stat(std::string &stat_error)
{
    int a = std::atoi(stat_error.c_str());
    if (a > 199 && a < 599) // you should add more i think ...
        return 0;
    return 1;
}

void        server::check_size(std::vector<std::string> &s, char c)
{
    if (c == 's')
    {
        if (!s_vec[0].compare("root"))
            s_root = s_vec[1];    
        if (!s[0].compare("listen") || !s[0].compare("root")
        || !s[0].compare("client_max_body_size") || !s[0].compare("host") 
        || !s[0].compare("server_name") || !s[0].compare("index"))
        {
            if (s.size() != 2)
                print_err("syntaxt_error " + s[0]);
        }
        else if (!s[0].compare("error_page") ){
            if (s.size() != 3)
                print_err("syntaxt_error " + s[0]);
        }
    }
    else
    {
        if ((!s[0].compare("root") || !s[0].compare("index") 
        || !s[0].compare("limit_except") || !s[0].compare("autoindex") 
        || !s[0].compare("upload")) || !s[0].compare("cgi_status") || !s[0].compare("return")){
            if (s.size() != 2)
                print_err("syntaxt_error on " + s[0]);
        }
        if (!s[0].compare("allow_methods") ){
            if (s.size() < 2 || s.size() > 4)
                print_err("syntaxt_error on allow_methods");
        }
        if (!s[0].compare("location"))
        {
            if (s.size() < 2 || s.size() > 2)
                print_err("syntaxt_error on location");
        }
        if (!s[0].compare("cgi_path"))
        {
            if (s.size() != 3)
                print_err("syntaxt_error on cgi_path");
        }
    }
}


void      server::handl_serv(std::vector<std::string> s)
{
    if (!s[0].compare("listen"))
    {   
        if (is_num(s[1]) || valid_range(s[1]))
            print_err("syntaxt_error on port");
    }
    else if (!s[0].compare("root"))
    {
        if (!check_exist(s[1]))
            print_err("syntaxt_error on the path");
    }
    else if (!s[0].compare("error_page"))
    {
        if (!check_exist(s[2]))
            print_err("syntaxt_error on the file");
    }
    else if (!s[0].compare("index"))
    {
        for (size_t i = 1; i < s.size(); i++)
        {
            indexs.push_back(s[i]);
            s[1] = s_root + "/" + s[i];
            if (!check_exist(s[1]))
                print_err("syntaxt_error on index");
        }
    }
    else if (!s[0].compare("host"))
    {
        if (check_ip(s[1]) )
            print_err("syntaxt_error on ip");
    }
    else if (!s[0].compare("client_max_body_size"))
        max_body = s[1];
}

int      server::check_ip_nbr(std::string nbr)
{
    int number = std::atoi(nbr.c_str());
    if (number < 0 || number > 255)
        return 1;
    return 0;
}

int      server::check_ip(std::string ip)
{
    int count = 0;
    for (size_t i = 0; i < ip.size(); i++)
    {
        if (ip[i] == '.')
            count++;
    }
    if (count != 3)
        return 1;
    std::vector<std::string> ip_nmbr = isolate_str(ip, '.');
    for (size_t i = 0; i < ip_nmbr.size(); i++)
    {
        if (is_num(ip_nmbr[i]) || check_ip_nbr(ip_nmbr[i]))
            return 1;
    }
    return 0;
}

void        server::stor_values(std::vector<std::string> s, char ch)
{
    if (ch == 's')
    {
        if (s[0].compare("error_page"))
        {
            std::map<std::string, std::string>::iterator it = cont.find(s[0]);
            if (it != cont.end())
                print_err("Duplicat on directive's server");
        }
        if (s[0].compare("error_page"))
            handl_serv(s);
        if (!s[0].compare("listen"))
            cont[s[0]] = s[1].substr(0, s[1].size());
        else if (!s[0].compare("error_page"))
            cont[s[0]] = s[1].substr(0, s[1].size());
        else if (!s[0].compare("host"))
            cont[s[0]] = s[1].substr(0, s[1].size());
        else if (!s[0].compare("server_name"))
            cont[s[0]] = s[1].substr(0, s[1].size());
        else if (!s[0].compare("client_max_body_size"))
            cont[s[0]] = s[1].substr(0, s[1].size());
        else if (!s[0].compare("index"))
            cont[s[0]] = s[1].substr(0, s[1].size());
    }
    else
    {
        duplicate.find(s[0])->second++;
        if (!s[0].compare("index"))            
            cont_l[s[0]] = s[1].substr(0, s[1].size());
        else if (!s[0].compare("root"))
            cont_l[s[0]] = controle_slash(s[1].substr(0, s[1].size()));
        else if (!s[0].compare("limit_except"))
            cont_l[s[0]] = s[1].substr(0, s[1].size());
        else if (!s[0].compare("allow_methods"))
            cont_l[s[0]] = s[1].substr(0, s[1].size());
        else if (!s[0].compare("autoindex"))
            cont_l[s[0]] = s[1].substr(0, s[1].size());
        else if (!s[0].compare("upload"))
            cont_l[s[0]] = s[1].substr(0, s[1].size());
        else if (!s[0].compare("upload_path"))
            cont_l[s[0]] = controle_slash(s[1].substr(0, s[1].size()));
        else if (!s[0].compare("cgi_status"))
            cont_l[s[0]] = s[1].substr(0, s[1].size());
    }
}


std::string    server::controle_slash(std::string direc) // remove slash beg, end .
{
    size_t  f_slash = 0;
    std::string normle_path;
    for (size_t i = 0; i < direc.size(); i++)
    {
        if (direc[i] == '/' && direc[i + 1] == '/')
            f_slash++;
        if (direc[i] == '/' && direc[i + 1] != '/')
            break;
    }
    normle_path = direc.substr(f_slash);
    size_t  lenth = normle_path.length();
    for (size_t len = direc.length(); len > 0; len--)
    {
        if (direc[len] == '/' && direc[len - 1] == '/')
            lenth--;
        if (direc[len] == '/' && direc[len - 1] != '/')
            break;
    }
    normle_path = normle_path.substr(0, lenth);
    if (normle_path[0] != '/' || normle_path.empty())
        normle_path = '/' + normle_path;
    if (normle_path[normle_path.length() - 1] != '/' || normle_path.empty())
        normle_path += '/';
    return (normle_path);
}