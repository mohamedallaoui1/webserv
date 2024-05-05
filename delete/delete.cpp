#include "../Client.hpp"
#include "../delete.hpp"

extern std::map<int, Client *> fd_maps;

std::string     delete_::delet_method(std::string path, server &server, int fd)
{
    std::string line;
    std::string path_;
    std::string res;
    struct stat path_stat;
    std::map<int, Client *>::iterator it = fd_maps.find(fd);

    stat(path.c_str(), &path_stat);
    
    if (access(path.c_str(), F_OK) < 0)
    {
        // std::cout << "file_exist === " << path << "\n";
        if (it->second->resp.response_error("404", fd))
            return ("delete_stat");
    }
    if (access(path.c_str(), W_OK) < 0)
    {
        if (it->second->resp.response_error("403", fd))
            return ("delete_stat");
    }
    if (!S_ISDIR(path_stat.st_mode)) // in case was a file .
    {
        if (!remove(path.c_str()))
            return ("delete");
        else
            return ("delete_failed");
    }
    else
    {
        DIR *direct = opendir(path.c_str());
        if (!direct)
        {
            if (it->second->resp.response_error("404", fd))
                return ("delete_stat");
        }
        else
        {
            struct dirent* entry = NULL;
            while ((entry = readdir(direct)))
            {
                if (std::string(entry->d_name).compare("..") && std::string(entry->d_name).compare("."))
                {
                    if (path[path.length() - 1] != '/' && it->second->get.check_exist(path + "/" + entry->d_name) == 2)
                    {
                        line = path + "/" + entry->d_name + "/";
                        delet_method(line, server, fd);
                    }
                    else /*if (path[path.length() - 1] != '/' && it->second->get.check_exist(path + "/" + entry->d_name) == 1)*/
                    {
                        line = path + "/" + entry->d_name;
                        delet_method(line, server, fd);
                    }
                }
            }
        }
        closedir(direct);
        if (!remove(path.c_str()))
            return ("delete");
        else
            return ("delete_failed");
    }
    return ("delete_failed");
}

delete_::delete_(){
    // std::cout << "Delete Method Constructor \n";

}

delete_::~delete_(){
    // std::cout << "Delete Method DeConstructor\n";
}