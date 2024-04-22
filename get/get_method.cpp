#include "../get_method.hpp"
#include "../Client.hpp"

extern std::map<int, Client> fd_maps;

get_method::get_method(){
    std::cout << "Get Constructor \n";
}

int    get_method::get_mthod(int fd)
{
    std::cout << "--------------- get_mthod --------------------------\n";
    std::map<int, Client>::iterator it = fd_maps.find(fd);
    std::string         response;
    std::string         extention_type;
    std::stringstream   StringSize;
    std::streampos      fileSize;
    std::string         buff_s;
    std::stringstream   size;
    int                 check_path;
    int                 err_stat;

    std::cout <<  "\t path inside get func = " << it->second.requst.uri << " *******************\n";
    std::cout <<  "\t autoindex stat = " << it->second.requst.auto_index_stat << " *******************\n";


    check_path = check_exist(it->second.requst.uri);

    std::cout <<  "\t path type = " << check_path<< " *******************\n";
    if (it == fd_maps.end()) // print error
        exit(1);
    fileSize = get_fileLenth(it->second.requst.uri); // get full lenth of the file
    extention_type = it->second.requst.get_exten_type(it->second.requst.uri);
    StringSize << fileSize;
    // it->second.message_response_stat();
    // err_page_();
    std::cout << "file lentgh : " << StringSize.str() << "\n";
    std::cout << "file Extention : " << extention_type << "\n";
    std::cout << "uri == " << it->second.requst.uri << "\n";
    std::cout << "check ===> " << check_path << " <===\n";

    if (check_path  == 2 && it->second.requst.redirct_loca)
    {
        std::cout << "check ===> " << check_path << " <===\n";
        std::cout << "redirct_loca ===> " << it->second.requst.redirct_loca << " <===\n";
        if (it->second.requst.path[it->second.requst.path.length() -1] != '/')
        {
            std::cout << it->second.requst.path << "\n";
            err_stat = it->second.resp.response_error("301", fd);
            if (err_stat)
                return 1;
        }
    }
    if (check_path == 1)
    {
        // std::cout << "stat =====" << it->second.requst.stat_cgi  << "\n";
        // std::cout << "x_cgi =====" << it->second.requst.x_cgi  << "\n";
        // if (!it->second.requst.x_cgi)
        // {
            if (!it->second.res_header)
            {
                std::cout << " File Case " << "\n";
                std::cout << "Before ResPonse <<-->> " << response << "\n";
                response = it->second.resp.get_header("200", extention_type, StringSize.str(), it->second);
                std::cout << "After ResPonse <<-->> " << response << "\n";
                it->second.read_f.open(it->second.requst.uri.c_str());
                send(fd, response.c_str(), response.size(), 0);
            }
            else
            {
                char            buff[1024];
                int     x = it->second.read_f.read(buff, 1024).gcount();
                if (it->second.read_f.gcount() > 0)
                    send(fd, buff, x, 0);
                if (it->second.read_f.eof() || it->second.read_f.gcount() < 1024)
                {
                    it->second.rd_done = 1;
                    return 1;
                }
            }
        // }
        // else if(it->second.requst.x_cgi && it->second.requst.stat_cgi == "on")
        // {
        //     //work on cgi
        //     it->second.cgi.cgi_work(fd);
        //     it->second.requst.x_cgi = 0;
        //     get_mthod(fd);
        //     std::cout << "1212222222222222\n";
        //     exit (12);
        // }
    }
    else if (check_path == 2 && it->second.requst.auto_index_stat)
    {
        std::cout << " Folder Case " << "\n";
        if (it->second.requst.uri[it->second.requst.uri.length() -1] != '/')
        {
            std::cout << it->second.requst.uri << "\n";
            err_stat = it->second.resp.response_error("301", fd);
            if (err_stat)
                return 1;
        }
        buff_s = generat_html_list(it->second.requst.uri.substr(0, it->second.requst.uri.find_last_of("/")));
        size << buff_s.size();

        if (!it->second.res_header)
        {
            response = it->second.resp.get_header("200", "text/html", size.str(), it->second);
            send(fd, response.c_str(), response.size(), 0);
            it->second.res_header = 1;
        }
        else if (it->second.res_header)
        { 
            send(fd, buff_s.c_str(), buff_s.size(), 0);
            it->second.rd_done = 1;
            return 1;
        }
    }
    else // generic function .
    { 
        std::cout << " Error Case " << "\n";
        if (check_path == 3) // permission
            err_stat = it->second.resp.response_error("403", fd);
        else                 // exist
            err_stat = it->second.resp.response_error("404", fd);
        if (err_stat)
            return 1;
    }
    return 0;
}


// std::map<std::string, std::string>        get_method::message_response_stat(/*std::map<std::string, std::string> &response_message*/)
// {
//     response_message["200"] = "OK";
//     response_message["201"] = "Created";
//     response_message["202"] = "Accepted";
//     response_message["204"] = "No Content";
//     response_message["301"] = "Moved Permanently";
//     response_message["302"] = "Found";
//     response_message["304"] = "Not Modified";
//     response_message["400"] = "Bad Request";
//     response_message["401"] = "Unauthorized";
//     response_message["403"] = "Forbidden";
//     response_message["404"] = "Not Found";
//     response_message["405"] = "Method Not Allowed";
//     response_message["500"] = "Internal Server Error";
//     response_message["505"] = "Version Not Supported";
//     response_message["501"] = "Not Implemented";
//     response_message["502"] = "Bad Gateway";
//     response_message["503"] = "Service Unavailable";
//     response_message["504"] = "Gateway Timeout";
//     return (response_message);
// }


int     get_method::response_error(std::string stat, int fd)
{
        std::string response;
        std::stringstream size;
        std::map<int, Client>::iterator it = fd_maps.find(fd);
        std::map<std::string, std::string>::iterator it_ = it->second.serv_.err_page.find(stat);
        std::map<std::string, std::string>::iterator it_message_err = response_message.find(stat);
        if( it_ != it->second.serv_.err_page.end())
        {
            std::fstream    err_file;
            err_file.open(it_->second.c_str());
            char            buff_[1024];
            err_file.read(buff_, 1024).gcount();
            response = buff_;
            size << response.size();
            response = get_header(stat, "text/html", size.str(), it->second);
            response += to_string(buff_);
            std::cout << "response -> " << response << " <-\n";
            send(fd, response.c_str(), response.size(), 0);
            it->second.rd_done = 1;
            return (1);
        }
        else
        {
            std::cout << "My Here \n";
            std::string _respond_stat;
            std::cout << "string error = " << it_message_err->second << "\n";
            _respond_stat = "<h1>" + it_message_err->second + "</h1>";
            _respond_stat += "<html><head><title> " + it_message_err->second + "</title></head>";
            size << _respond_stat.size();
            response = get_header(stat, "text/html", size.str(), it->second);
            response += _respond_stat;
            send(fd, response.c_str(), response.size(), 0);
            it->second.rd_done = 1;
            return (1);
        }
        return (0);
}

std::string     get_method::get_exten_type(std::string path, std::map<std::string, std::string> &exta)
{
    std::string exten;
    size_t      pos = path.find_last_of(".");
    exten = path.substr(pos + 1);
    if (!exten.empty())
        return ("application/octet-stream");
    std::map<std::string, std::string>::iterator b = exta.find(exten);
    if (b != exta.end())
        return ((*b).second);
    return ("Unsupported");
}

std::streampos  get_method::get_fileLenth(std::string path)
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

std::string      get_method::get_header(std::string wich, std::string exten, std::string lentg, Client& fd_inf)
{
    std::string response;
    std::map<std::string , std::string>::iterator it = response_message.find(wich);
    std::cout << "stat " << wich << " \n";
    if (it != response_message.end())
    {
        std::cout << "stat Found 0_0 " << wich << " \n";
        if (!wich.compare("200") || !wich.compare("404") || !wich.compare("403"))
        {
            response = "HTTP/1.1 ";
            response +=  it->first + " " + it->second + "\r\n";
            response += "Content-Type: " + exten + "\r\n" + "Content-Length: " + lentg + "\r\n\r\n";
            fd_inf.res_header = 1;
            return (response);
        }
        else if (wich == "301")
        {
            std::cout << "hi hahahahahah \n";
            std::string     path_with_slash = fd_inf.requst.path + "/";
            response = "HTTP/1.1 301 Moved Permanently\r\n";
            response += "Location: " + path_with_slash + "\r\n\r\n";
            fd_inf.res_header = 1;
            return (response);
        }
    }
    return "";
}

std::string    get_method::generat_html_list(std::string directory)
{
    std::string resp;

    DIR *dir = opendir(directory.c_str());

    if (dir)
    {
        resp += "<html><head><title>Index of " + directory + " </title></head><body>";
        resp += "<h1>Index of " + directory + "</h1><hr>";

        struct dirent* entry;
        while ((entry = readdir(dir)))
        {
            if (std::string(entry->d_name).compare(".") && std::string(entry->d_name).compare(".."))
                resp += "<a href=\""+ std::string(entry->d_name) + " \">" + std::string(entry->d_name) + "</a><br>";
        }
        resp += "<hr></body></html>";
        closedir(dir);
    }
    else
    {
        perror("Folder Not Found");
        exit(1);
    }
    return resp;
}

std::string            get_method::get_index_file(std::map<std::string, std::string> &loca_map)
{
    std::map<std::string, std::string>::iterator it_e = loca_map.end();
    for (std::map<std::string, std::string>::iterator it_b = loca_map.begin(); it_b != it_e; it_b++)
    {
        if (!(*it_b).first.compare("index"))
        {
            std::cout << "mkhaskch tdkhl yahad wld ..\n";

            return ((*it_b).second);
        }
    }
    return (0);
}

bool            get_method::check_autoindex(std::map<std::string, std::string> loca_map)
{
    std::map<std::string, std::string>::iterator it_e = loca_map.end();
    for (std::map<std::string, std::string>::iterator it_b = loca_map.begin(); it_b != it_e; it_b++)
    {
        if (!(*it_b).first.compare("autoindex"))
        {
            if (!(*it_b).second.compare("on"))
                checki = true;
            break;
        }
    }
    return (checki);
}

int     get_method::check_exist(const std::string& path) 
{
    struct stat fileStat;
    if (stat(path.c_str(), &fileStat) == 0) 
    {
        if ((fileStat.st_mode & S_IROTH) == 0)  // User does not have read permission
            return 3; // Path exists but user doesn't have permission
        if (S_ISREG(fileStat.st_mode)) 
            return 1; // Path exists and is a regular file
        if (S_ISDIR(fileStat.st_mode))
            return 2; // Path exists and is a directory
    }
    return 0; // Path does not exist
}

