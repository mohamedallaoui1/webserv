#include "Client.hpp"
#include "cgi.hpp"
#include "multplixing.hpp"
extern std::map<int, Client> fd_maps;

cgi::cgi(){
    std::cout << "Cgi Constructor \n";
}

void        cgi::fill_env_cgi(Client &obj)
{
    // SCRIPT_NAME = ;
    // QUERY_STRING    = ;
    // CONTENT_LENGTH  = ;
    idx = 0;
    cgi_env.push_back("REQUEST_METHOD=" + obj.requst.method);
    cgi_env.push_back("CONTENT_TYPE=text/plain");
    cgi_env.push_back("SCRIPT_FILENAME=" + obj.requst.uri);
    cgi_env.push_back("REDIRECT_STATUS=200OK");
    cgi_env.push_back("SERVER_PORT=8080");//change it !!
    std::vector<std::string>::iterator  ite = cgi_env.end();
    for(std::vector<std::string>::iterator  itb = cgi_env.begin(); itb != ite; itb++)
        env[idx++] = strdup(itb->c_str());
    env[idx] = NULL;
}

void        cgi::cgi_work(int fd)
{
    std::map<int, Client>::iterator it = fd_maps.find(fd);
    std::cout <<"hahhahahhahahahha" << it->second.requst.uri << "\n";
    fill_env_cgi(it->second);

    int fd_out = open("file.txt", O_RDWR | O_CREAT, 0666);

    if (fd_out < 1)
        it->second.serv_.print_err("file error");
    if (dup2(fd_out, 1) == -1) 
        it->second.serv_.print_err("dup error");
    close(fd_out);
    // char* pythonScriptPath = requestedFilePath;
    char* pythonInterpreter = strdup("/usr/bin/python3");
    char* args[] = {pythonInterpreter, strdup(it->second.requst.uri.c_str()), NULL};

    if (execve(pythonInterpreter, args, env) == -1) 
        it->second.serv_.print_err("execve error");
    // std::ofstream   cgi_file("file.txt");
    exit (12);
}

// void     cgi::get_exten_type(int fd)
// {s
//     std::cout << "\nCGI Function"<< "\n";
//     std::string exten;
//     size_t      pos = it->second.requst.uri.find_last_of(".");

//     std::cout << "uri = " << it->second.requst.uri << "\n";

//     exten = it->second.requst.uri.substr(pos + 1);

//     std::map<std::string, std::string>::iterator it_bi = it->second.serv_.cgi_map.begin();
//     std::map<std::string, std::string>::iterator it_end = it->second.serv_.cgi_map.end();
//     while (it_bi != it_end)
//     {
//         std::cout << "first = " << it_bi->first << "\n";
//         std::cout << "second = " << it_bi->second << "\n";
//         it_bi++;
//     }
//     exit (21);
//     std::map<std::string, std::string>::iterator b_conf = it->second.serv_.cgi_map.find(exten);
//     std::map<std::string, std::string>::iterator b_uri = it->second.requst.extentions.find(exten);

//     if (b_uri != it->second.requst.extentions.end())
//         cgi_stat = "static_file";
//     if (b_conf != it->second.serv_.cgi_map.end())
//         cgi_stat = "cgi_state";
//     std::cout << "exten == " << exten << "\n";
//     std::cout << "cgi_stat == " << cgi_stat << "\n";
//     exit (20);
// }

void    cgi::checkifcgi(request& rq, int& iscgi, int fd) {
    std::string path = rq.uri;
    std::string::iterator it = path.begin() + path.find_last_of("/") + 1;
    if (it == path.end()) {
        std::cout << "\033[1;38;5;201mTHIS IS NOT CGI, THIS IS A FOLDER\033[0m" << std::endl;
        iscgi = 0;
        stat_cgi = 0;
        return ;
    }

    std::string file = std::string(it, path.end());
    if (fd_maps[fd].requst.cgi_map.find(file.substr(file.find_last_of(".") + 1)) != fd_maps[fd].requst.cgi_map.end()) {
        iscgi = 1;
        stat_cgi = 1;
        compiler = fd_maps[fd].requst.cgi_map[file.substr(file.find_last_of(".") + 1)];
        extension = file.substr(file.find_last_of(".") + 1);
        std::cout << "\033[1;38;5;82mTHIS IS CGI, yaaaY " << compiler << "\033[0m" << std::endl;
    }
}

char **cgi::fillCgiEnv(int fd) {
    std::vector<std::string> env_v;
    env_v.push_back("SCRIPT_NAME=" + compiler);
    env_v.push_back("REQUEST_METHOD=" + fd_maps[fd].requst.method);
    env_v.push_back("REDIRECT_STATUS=CGI");
    env_v.push_back("PATH_TRANSLATED=" + fd_maps[fd].requst.uri);
    env_v.push_back("QUERY_STRING=");
    env_v.push_back("HTTP_COOKIE=");
    char **env = new char*[env_v.size() + 1];
    for (std::vector<std::string>::iterator it = env_v.begin(); it != env_v.end(); it++) {
        env[it - env_v.begin()] = strdup(it->c_str());
    }
    env[env_v.size()] = NULL;
    return env;
}

void    cgi::cgi_method(request& rq, int fd) {
    std::cout << "\033[1;31mI AM ABOUT TO FORK\033[0m" << std::endl;
    std::stringstream iss;
    iss << time(NULL);
    std::string name;
    iss >> name;
    file_out ="/tmp/" + name;
    file_in = "/tmp/" + name + ".in";
    char **env = fillCgiEnv(fd);
    char **args = new char*[3];
    start_time = time(NULL);
    clientPid = fork();
    if (clientPid == 0) {
        freopen(file_out.c_str(), "w", stdout);
        freopen(file_out.c_str(), "w", stderr);
        freopen(file_in.c_str(), "r", stdin);
        // print with bold red "I AM IN THE CHILD PROCCESS"
        args[0] = strdup(compiler.c_str());
        args[1] = strdup(rq.uri.c_str());
        args[2] = NULL;
        execve(args[0], args, env);
        std::cerr << "\033[1;31mI AM IN THE CHILD PROCCESS\033[0m" << std::endl;
        kill(getpid(), 2);
    }
    std::cout << "\033[1;33m-----------------------------------\033[0m" << std::endl;
}

cgi::~cgi(){
}