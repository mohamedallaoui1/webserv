#include "../location.hpp"

location::location(std::map<std::string, std::string> &c, std::vector <std::string> &v_s, std::map <std::string, std::string> &cgi_m, std::string &redirc)
{
    cont_l = c;
    allowed_methods = v_s;
    cgi_map = cgi_m;
    redirction_path = redirc;
}

int     location::check_exist(std::string path)
{
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) == 0) 
    {
        if (S_ISREG(fileStat.st_mode) || S_ISDIR(fileStat.st_mode))
            return 1; // Path exists and is a regular file
    }
    return 0;
} 

void            location::print_err(std::string str)
{
    std::cout << str << std::endl;
    exit(1);
}

void        location::check_dup()
{
    std::map<std::string, int>::iterator it = duplicate.begin();
    std::map<std::string, int>::iterator ite = duplicate.end();
    for(;it != ite; it++)
    {
        if (!it->first.compare("cgi_status"))
        {
            if (it->second > 1)
                print_err("duplicate in directive");
            it->second = 0;
        }
        if (!it->first.compare("root"))
        {
            if (it->second != 1)
                print_err("duplicate in directive");
            it->second = 0;
        }
        if (!it->first.compare("allow_methods"))
        {
            if (it->second > 1)
                print_err("duplicate in directive");
            it->second = 0;
        }
        if (!it->first.compare("autoindex"))
        {
            if (it->second > 1)
                print_err("duplicate in directive");
            it->second = 0;
        }
        if (!it->first.compare("index"))
        {
            if (it->second > 1)
                print_err("duplicate in directive");
            it->second = 0;
        }
    } 
}

void        location::handl_loca(std::map<std::string, std::string>& m, std::vector<std::string> &methods, std::string root)
{

    std::map<std::string, std::string>::iterator it = m.begin();
    std::map<std::string, std::string>::iterator ite = m.end();

    while (it != ite)
    {
        if (!(*it).first.compare("root"))
        {
            if (!check_exist((*it).second))
                print_err("syntaxt_error on the folder");
        }
        if (!(*it).first.compare("upload") || !(*it).first.compare("autoindex") || !(*it).first.compare("cgi_status"))
        {
            if (((*it).second.compare("on") && (*it).second.compare("off")))
                print_err("syntaxt_error must be 'on' or 'off'");
        }
        if (!(*it).first.compare("upload_path"))
        {
            if (!check_exist((*it).second))
                print_err("syntaxt_error on upload_path's path");
        }
        if (!(*it).first.compare("allow_methods"))
        {
            for (size_t i = 1; i < methods.size(); i++)
            {
                if (methods[i].compare("GET") && methods[i].compare("DELETE")
                && methods[i].compare("POST"))
                    print_err("syntaxt_error methods");
            }
        }
        if (!(*it).first.compare("index"))
        {
            std::string path = root + "/" + (*it).second;
            if (!check_exist(path))
                print_err("syntaxt_error index file");
        }
        it++;
    }
}

void        location::cgi_extention()
{
    std::map<std::string, std::string>::iterator it = cgi_map.begin();
    std::map<std::string, std::string>::iterator ite = cgi_map.end();
    while (it != ite)
    {
        if (it->first.compare("py") && it->first.compare("php")
        && it->first.compare("pl") && it->first.compare("sh"))
            print_err("Extention Cgi Error");
        if (!check_exist(it->second))
            print_err("Cgi Path Not Found");
        it++;
    }
}

location::location(){
    
}