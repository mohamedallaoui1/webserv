#include "headers/request.hpp"
#include <sys/select.h>
#include "headers/Client.hpp"
#include "headers/multplixing.hpp"

size_t MAX_CLIENTS = 10;

int main(int ac, char **av) 
{
    server              parse;
    request             rq;
    multplixing         mlp;
    if (ac < 2)
    {
        std::string def = "config/def.conf";
        std::ifstream av_file(def.c_str());
        if (!av_file.is_open())
            parse.print_err("Config File Not Found");
        parse.mange_file(def.c_str());
    }
    else if (ac == 2)
    {
        std::string avv = av[1];
        std::ifstream av_file(avv.c_str());
        if (!av_file.is_open())
            parse.print_err("Config File Not Found");
        parse.mange_file(avv.c_str());
    }
    else
        parse.print_err("Argement Not Valid");
    mlp.lanch_server(parse);
}