#ifndef POST_HPP
#define POST_HPP

#include <string>
#include <sys/time.h>
#include <cstring>
#include <cstdlib>
#include <sys/wait.h>
#include <iostream>
#include <cstring>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <map>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include "server.hpp"
#include "request.hpp"
#include "response.hpp"


#define PORT 8081

typedef std::map<std::string, std::string> map;

class post
{
private:

public:
    std::string transfer_encoding;
    std::string content_length;
    std::string content_type;
    int g;
    int j;
	post();
	post(const post &other);
	post &operator=(const post &other);
	~post();
    std::string generateUniqueFilename();
    void print_keyVal(map m);
    map read_file_extensions(const char *filename);
    void parse_header(std::string buffer);
    void PutBodyInFile(std::string buffer, std::string extension);
    bool post_method(std::string buffer, int fd);
    bool binary(std::string buffer, std::string max_body_size, std::string upload_path);
    bool chunked(std::string buffer, std::string max_body_size, std::string upload_path);
    bool extension_founded(std::string contentType);
    void parse_hexa(std::string &remain);
    bool is_end_of_chunk(std::string max_body_size, std::string upload_path);
    bool boundary(std::string buffer);
    std::string parse_boundary_header(std::string buffer);
    std::string cat_header(std::string buffer);
    bool containsValidCharacters(const std::string& str);
};


#endif