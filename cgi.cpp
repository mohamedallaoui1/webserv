#include "Client.hpp"
#include "cgi.hpp"
#include "multplixing.hpp"
extern std::map<int, Client *> fd_maps;
extern std::vector<void *> garbage;

cgi::cgi(){
    // // std::cout << "Cgi Constructor \n";
}

void    cgi::checkifcgi(request& rq, int& iscgi, int fd) {
    std::string path = rq.uri;
    std::string::iterator it = path.begin() + path.find_last_of("/") + 1;
    if (it == path.end()) {
        // //"\033[1;38;5;201mTHIS IS NOT CGI, THIS IS A FOLDER\033[0m" << std::endl;
        iscgi = 0;
        stat_cgi = 0;
        return ;
    }

    std::string file = std::string(it, path.end());
    if (fd_maps[fd]->requst->cgi_map.find(file.substr(file.find_last_of(".") + 1)) != fd_maps[fd]->requst->cgi_map.end()) {
        iscgi = 1;
        stat_cgi = 1;
        compiler = fd_maps[fd]->requst->cgi_map[file.substr(file.find_last_of(".") + 1)];
        extension = file.substr(file.find_last_of(".") + 1);
        // //"\033[1;38;5;82mTHIS IS CGI, yaaaY " << compiler << "\033[0m" << std::endl;
    }
}

char **cgi::fillCgiEnv(int fd) {
    std::vector<std::string> env_v;
    env_v.push_back("SCRIPT_NAME=" + compiler);
    env_v.push_back("REQUEST_METHOD=" + fd_maps[fd]->requst->method);
    env_v.push_back("REDIRECT_STATUS=CGI");
    env_v.push_back("PATH_TRANSLATED=" + fd_maps[fd]->requst->uri);
    env_v.push_back("QUERY_STRING=");
    env_v.push_back("HTTP_COOKIE=" + fd_maps[fd]->cgi_.HTTP_COOKIE);
    char **env = new char*[env_v.size() + 1];
    for (std::vector<std::string>::iterator it = env_v.begin(); it != env_v.end(); it++) {
        // print with bold blue the value of the env variable
        env[it - env_v.begin()] = strdup(it->c_str());
    }
    env[env_v.size()] = NULL;
    return env;
}

void    cgi::cgi_method(request& rq, int fd) {
    // //"\033[1;31mI AM ABOUT TO FORK\033[0m" << std::endl;
    std::stringstream iss;
    iss << time(NULL);
    std::string name;
    iss >> name;
    file_out ="/tmp/" + name;
    file_in = "/tmp/" + name + ".in";
    char **env = fillCgiEnv(fd);
    char **args = new char*[3];
    garbage.push_back(env);
    garbage.push_back(args);
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
        std::string current_path = rq.uri.substr(0, rq.uri.find_last_of("/"));
        chdir(current_path.c_str());
        execve(args[0], args, env);
        std::cerr << "\033[1;31mI AM IN THE CHILD PROCCESS\033[0m" << std::endl;
        kill(getpid(), 2);
    }
    // //"\033[1;33m-----------------------------------\033[0m" << std::endl;
}


cgi::~cgi(){
}