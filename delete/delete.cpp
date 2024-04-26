#include "../Client.hpp"
#include "../delete.hpp"

extern std::map<int, Client> fd_maps;

std::string     delete_::delet_method(std::string path, server &server, int fd)
{
    std::string line;
    std::string path_;
    std::string res;
    struct stat path_stat;
    std::map<int, Client>::iterator it = fd_maps.find(fd);

    stat(path.c_str(), &path_stat);
    
    if (access(path.c_str(), W_OK) < 0)
    {
        if (it->second.resp.response_error("403", fd))
            return ("delete_stat");
    }
    if (access(path.c_str(), F_OK) < 0)
    {
        std::cout << "file_exist === " << path << "\n";
        if (it->second.resp.response_error("404", fd))
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
            if (it->second.resp.response_error("404", fd))
                return ("delete_stat");
        }
        else
        {
            struct dirent* entry = NULL;
            while ((entry = readdir(direct)))
            {
                if (std::string(entry->d_name).compare("..") && std::string(entry->d_name).compare("."))
                {
                    if (path[path.length() - 1] != '/' && it->second.get.check_exist(path + "/" + entry->d_name) == 2)
                    {
                        line = path + "/" + entry->d_name + "/";
                        delet_method(line, server, fd);
                    }
                    else /*if (path[path.length() - 1] != '/' && it->second.get.check_exist(path + "/" + entry->d_name) == 1)*/
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

// std::string     delete_::delet_method(std::string path, server &server, int fd)
// {
//     std::string line;
//     std::string path_;

//     std::cout << "Path == " << path << "\n";

//     struct stat path_stat;
//     int stat_ = stat(path.c_str(), &path_stat);
//     std::string res;

//     std::map<int, Client>::iterator it = fd_maps.find(fd);

//     if(stat_)
//         res = get_delet_resp(path, 4, it->second);
    
//     if (!S_ISDIR(path_stat.st_mode))
//     {
//         if (!remove(path.c_str()))
//         {
//             res = get_delet_resp(path, 1, it->second);
//             std::cout << "haa9 Mcha ----> " << "Path =" << path << "\n";
//             return (res);
//         }
//         else
//         {
//             res = get_delet_resp(path, 4, it->second);
//             std::cout << "haa9 Mcha ----> " << "Path =" << path << "\n";
//             std::cout << "\nMabghach Ya Mchi \n";
//             return (res);
//         }
//     }
//     else
//     {
//         DIR *direct = opendir(path.c_str());
//         if (!direct)
//         {
//             perror("Folder does'nt exist");
//             exit (1);
//         }
//         else
//         {
//             std::cout << "Here \n";
//             struct dirent* entry = NULL;
//             while ((entry = readdir(direct)))
//             {
//                 std::cout << "Here 2 " << entry->d_name << "\n";
//                 if (std::string(entry->d_name).compare("..") && std::string(entry->d_name).compare("."))
//                 {
//                     std::cout << "check  < -- > : " << path << "\n"; 
//                     if (path[path.length() - 1] != '/')
//                     {
//                         std::cout << "entre 1"<< "\n";
//                         line = path + "/" + entry->d_name;
//                         delet_method(line, server, fd);
//                     }
//                     else
//                     {
//                         std::cout << "entre 2"<< "\n";
//                         line = path + entry->d_name;
//                         delet_method(line, server, fd);
//                     }
//                 }
//             }
//         }
//         closedir(direct);
//         if (!remove(path.c_str()))
//         {
//             std::cout << " path "  << path <<" Is Deleted Successfull !\n";
//             return (get_delet_resp(path, 1, it->second));
//         }
//         else
//         {
//             std::cout <<  " path " << path <<"Is Not Deleted Successfull !\n";
//             return (get_delet_resp(path, 4, it->second));
//         }
//     }
//     return ("---");
// }

delete_::delete_(){
    // std::cout << "Delete Method Constructor \n";

}

delete_::~delete_(){
    // std::cout << "Delete Method DeConstructor\n";
}