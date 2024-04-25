#include "request.hpp"
#include <sys/select.h>
#include "Client.hpp"
#include "multplixing.hpp"

// int flag = 0;
size_t MAX_CLIENTS = 10;
// int BUFFER_SIZE = 1024;

int main(int ac, char **av) 
{
    server              parse;
    request             rq;
    multplixing         mlp;

    if (ac < 2)
        parse.mange_file("def.conf");
    else if (ac == 2)
        parse.mange_file(av[1]);
    else
        parse.print_err("Argement Not Valid");
    // exit(22);
    mlp.lanch_server(parse);
}