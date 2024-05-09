#include "../headers/get_method.hpp"
#include "../headers/Client.hpp"
#include "../headers/multplixing.hpp"
# include "../headers/cgi.hpp"

int checkcontent = 0;
extern std::map<int, Client *> fd_maps;
std::string cookie = "";
size_t off = 0;

get_method::get_method(){
}

std::string parsephpheader(std::string buffer)
{
    std::istringstream stream (buffer);
    std::string line;
    std::string contenttype;
    int check = 0;
    while (getline(stream, line))
    {
        if (line.substr(0, 12) == "Content-Type") {
            contenttype = line.substr(14);
            check = 1;
        }
        if (line.substr(0, 10) == "Set-Cookie") {
            cookie = line.substr(12);
        }
    }
    if (!check)
        contenttype = "text/html";
    return contenttype;
}


std::string getphpstatus(std::string &content) {
    std::istringstream stream (content);
    std::string line;
    std::string status;
    while (getline(stream, line))
    {
        if (line.substr(0, 6) == "Status") {
            status = line.substr(8);
            return status;
        }
    }
    return "200 OK";
}

std::string getContent (std::string file, int fd,  std::string& contenttype, int c) {
    std::string content;
    std::ifstream fileStream;
    fileStream.open(file.c_str());
    if (fileStream.is_open()) {
        std::stringstream sstr;
        sstr << fileStream.rdbuf();
        content = sstr.str();
        fileStream.close();
    }
    if (c)
        return getphpstatus(content);
    if (fd_maps[fd]->cgi_.extension == "php" && !fd_maps[fd]->cgi_.is_error) {
        size_t pos = content.find("\r\n\r\n");
        contenttype = parsephpheader(content.substr(0, pos));
        if (pos != std::string::npos) {
            content = content.substr(pos + 4);
            return content;
        }
    }
    else {
        if (!checkcontent)
            contenttype = "text/html";
    }
    return content;
}


std::string getErrorPage(int fd, std::string stat, std::string& status, std::string& contenttype) {
    if (stat == "500")
        status = "500 Internal Server Error";
    else if (stat == "504")
        status = "504 Gateway Timeout";
    if (fd_maps[fd]->serv_.err_page.find(stat) != fd_maps[fd]->serv_.err_page.end()) {
        contenttype = fd_maps[fd]->requst.extentions[fd_maps[fd]->serv_.err_page[stat].substr(fd_maps[fd]->serv_.err_page[stat].find_last_of(".") + 1)];
        return getContent(fd_maps[fd]->serv_.err_page[stat], fd,  contenttype, 0);
    }
    else {
        contenttype = "text/html";
        if (stat == "500")
            return "500 Internal Server Error";
        else if (stat == "504")
            return "504 Gateway Timeout";
    }
    return "";
}

void cgi::sendResponse(int fd, std::string& response, std::string stat, std::string& contenttype) {
    std::stringstream iss;
    std::string status = "200 OK";
    if (!fd_maps[fd]->completed) {
        if (fd_maps[fd]->cgi_.is_error) {
            checkcontent = 1;
            response = getErrorPage(fd, stat, status, contenttype);
        }
        if (fd_maps[fd]->requst.uri.substr(fd_maps[fd]->requst.uri.find_last_of(".") + 1) == "php")
            status = getContent(fd_maps[fd]->cgi_.file_out,fd,  contenttype, 1);
        iss << response.length();
        std::string responseLength = iss.str();
        std::string httpResponse = "HTTP/1.1 " + status + "\r\n";
        if (cookie != "")
            httpResponse += "Set-Cookie: " + cookie + "\r\n";
        httpResponse += "Content-Type: " + contenttype + "\r\n";
        httpResponse += "Content-Length: " + responseLength + "\r\n";
        httpResponse += "\r\n";
        if (fd_maps[fd]->cgi_.is_error)
            httpResponse += response; 
        send(fd, httpResponse.c_str(), httpResponse.length(), 0);
        fd_maps[fd]->completed = 1;
        if (fd_maps[fd]->cgi_.is_error)
            multplixing::close_fd(fd, fd_maps[fd]->epoll_fd);

    }
    else if (fd_maps[fd]->completed) {
        char    buff[1024];
        std::stringstream ss(response);
        ss.ignore(off);
        int x = ss.read(buff, 1024).gcount();
        if (x > 0) {
            off += x;
            std::cout << off << std::endl;
            send(fd, buff, x, 0);
        }
        if (ss.eof() || ss.gcount() < 1024) {
            off = 0;
            multplixing::close_fd(fd, fd_maps[fd]->epoll_fd);
            return ;
        }
    }
}

int  cgiresponse(int fd) {
    std::string         contenttype;
    cookie = "";
    time_t end = time(NULL);
    std::string cgi_file = fd_maps[fd]->cgi_.file_out; // 0000 update
    int checkex;
    int wait = waitpid(fd_maps[fd]->cgi_.clientPid, &checkex, WNOHANG);
    if (wait == fd_maps[fd]->cgi_.clientPid) {
        std::cout << fd_maps[fd]->cgi_.stat_cgi << " | " << fd_maps[fd]->completed << std::endl;
        std::string content;
        checkcontent = 0;
        fd_maps[fd]->cgi_.is_error = 0;
        content = getContent(cgi_file,fd,  contenttype, 0);
        if (WIFSIGNALED(checkex) || checkex) {
            fd_maps[fd]->cgi_.is_error = 1;
            cgi::sendResponse(fd, content, "500", contenttype);
            isfdclosed = true;
            return 1;
        }
        else {
            fd_maps[fd]->cgi_.is_error = 0;
            cgi::sendResponse(fd, content, "200", contenttype);
            isfdclosed = true;
            return 1;
        }
    }
    else if (fd_maps[fd]->cgi_.stat_cgi && fd_maps[fd]->completed) {
        std::cout << "I AM HERE\n";
        fd_maps[fd]->cgi_.is_error = 0;
        std::string content = getContent(cgi_file,fd,  contenttype, 0);
        cgi::sendResponse(fd, content, "200", contenttype);
        isfdclosed = true;
        return 1;
    }
    else if (difftime(end, fd_maps[fd]->cgi_.start_time) > 7) {
        fd_maps[fd]->completed = 0;
        fd_maps[fd]->cgi_.is_error = 1;
        std::string timeout = "GATEWAY TIMEOUT";
        kill(fd_maps[fd]->cgi_.clientPid, 9);
        waitpid(fd_maps[fd]->cgi_.clientPid, NULL, 0);
        cgi::sendResponse(fd, timeout, "504", contenttype);
        isfdclosed = true;
        return 1;
    }
    else {
        fd_maps[fd]->rd_done = 0;
        return 0;
    }
    return 0;
}

int    get_method::get_mthod(int fd)
{
    std::map<int, Client *>::iterator it = fd_maps.find(fd);
    std::string         response;
    std::string         extention_type;
    std::stringstream   StringSize;
    std::streampos      fileSize;
    std::string         buff_s;
    std::stringstream   size;
    int                 check_path;
    int                 err_stat;

    check_path = check_exist(it->second->requst.uri);

    if (it == fd_maps.end()) // print error
        exit(1);
    fileSize = get_fileLenth(it->second->requst.uri); // get full lenth of the file
    extention_type = it->second->requst.get_exten_type(it->second->requst.uri);
    StringSize << fileSize;

    if (check_path  == 2 && it->second->requst.redirct_loca)
    {
        if (it->second->requst.path[it->second->requst.path.length() -1] != '/')
        {
            err_stat = it->second->resp.response_error("301", fd);
            if (err_stat)
                return 1;
        }
    }
    if (check_path == 1)
    {
        if (fd_maps[fd]->cgi_.stat_cgi)
            return cgiresponse(fd);
        if (!it->second->res_header) {
            response = it->second->resp.get_header("200", extention_type, StringSize.str(), *it->second);
            it->second->read_f.open(it->second->requst.uri.c_str());
            send(fd, response.c_str(), response.size(), 0);
        }
        else
        {
            char    buff[1024];
            int     x = it->second->read_f.read(buff, 1024).gcount();
            if (it->second->read_f.gcount() > 0) {
                send(fd, buff, x, 0);
            }
            if (it->second->read_f.eof() || it->second->read_f.gcount() < 1024)
            {
                it->second->rd_done = 1;
                return 1;
            }
        }
    }
    else if (check_path == 2 && it->second->requst.auto_index_stat)
    {
        if (it->second->requst.uri[it->second->requst.uri.length() -1] != '/')
        {
            err_stat = it->second->resp.response_error("301", fd);
            if (err_stat)
                return 1;
        }
        buff_s = generat_html_list(it->second->requst.uri.substr(0, it->second->requst.uri.find_last_of("/")));
        size << buff_s.size();
        if (!it->second->res_header)
        {
            response = it->second->resp.get_header("200", "text/html", size.str(), *it->second);
            send(fd, response.c_str(), response.size(), 0);
            fd_maps[fd]->start_time = time(NULL);
            it->second->res_header = 1;
        }
        else if (it->second->res_header)
        { 
            send(fd, buff_s.c_str(), buff_s.size(), 0);
            fd_maps[fd]->start_time = time(NULL);
            it->second->rd_done = 1;
            return 1;
        }
    }
    else
    { 
        err_stat = 0;
        if (check_path == 3 || (check_path == 2  && !it->second->requst.auto_index_stat)) // permission
            err_stat = it->second->resp.response_error("403", fd);
        if (access(it->second->requst.uri.c_str(), F_OK) < 0)
             err_stat = it->second->resp.response_error("404", fd);
        if (err_stat)
            return 1;
    }
    return 0;
}

int     get_method::response_error(std::string stat, int fd)
{
        std::string response;
        std::stringstream size;
        std::map<int, Client *>::iterator it = fd_maps.find(fd);
        std::map<std::string, std::string>::iterator it_ = it->second->serv_.err_page.find(stat);
        std::map<std::string, std::string>::iterator it_message_err = response_message.find(stat);
        if( it_ != it->second->serv_.err_page.end())
        {
            std::fstream    err_file;
            err_file.open(it_->second.c_str());
            char            buff_[1024];
            err_file.read(buff_, 1024).gcount();
            response = buff_;
            size << response.size();
            response = get_header(stat, "text/html", size.str(), *it->second);
            response += to_string(buff_);
            send(fd, response.c_str(), response.size(), 0);
            it->second->rd_done = 1;
            return (1);
        }
        else
        {
            std::string _respond_stat;
            _respond_stat = "<h1>" + it_message_err->second + "</h1>";
            _respond_stat += "<html><head><title> " + it_message_err->second + "</title></head>";
            size << _respond_stat.size();
            response = get_header(stat, "text/html", size.str(), *it->second);
            response += _respond_stat;
            send(fd, response.c_str(), response.size(), 0);
            it->second->rd_done = 1;
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
    if (it != response_message.end())
    {
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
        if ((fileStat.st_mode & S_IROTH) == 0)
            return 3;
        if (S_ISREG(fileStat.st_mode)) 
            return 1;
        if (S_ISDIR(fileStat.st_mode))
            return 2;
    }
    return 0;
}

