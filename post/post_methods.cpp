#include "../Client.hpp"

extern std::map<int, Client>  fd_maps;

/*-- My Global variables --*/

std::ofstream outFile;
std::string extension;
std::string file = "";

// for binary;
ssize_t body_size = 0;

// for chunked;
size_t chunk_length = 0;
std::stringstream ss;
std::string hexa;
std::string concat;
int chunked_len = 0;
int f = 0;

post::post()
{
    // std::cout << "Default constructor called\n";
}

post::post(const post &other)
{
    // std::cout << "Copy constructor called\n";
    *this = other;
}

post &post::operator=(const post &other)
{
    (void)other;
    // std::cout << "Copy assignment operator called\n";
    // if (this != &other)
    return *this;
}

post::~post()
{
    // std::cout << "Destructor called\n";
}

bool post::is_end_of_chunk(std::string max_body_size, std::string upload_path)
{
    if (concat.find("\r\n0\r\n\r\n") != std::string::npos || chunk_length == 0)
    {
        outFile << concat.substr(0, concat.find("\r\n0\r\n\r\n"));
        chunked_len += concat.substr(0, concat.find("\r\n0\r\n\r\n")).length();
        outFile.close();
        outFile.clear();
        concat.clear();
        f = 0;
        if (chunked_len > atoi(max_body_size.c_str()))
        {
            // std::cout << "so????\n";
            g = 3;
            remove((upload_path + file).c_str());
            chunked_len = 0;
            return true;
        }
        // std::cout << "done;\n";
        return true;
    }
    return false;
}

bool post::extension_founded(std::string contentType)
{
    extension = "";
    map m = read_file_extensions("fileExtensions");
    map::iterator it = m.find(contentType);
    // std::cout <<"$#" <<contentType << "$%" << std::endl;
    if (it != m.end())
        extension = it->second;
    else
    {
        // std::cerr << "extension not found\n";
        return false;
    }
    return true;
}

std::string sep = "";

bool post::post_method(std::string buffer, int fd)
{
    std::map<int, Client>::iterator   it_ = fd_maps.find(fd);
    // std::cout << "Upload_path = " << it_->second.requst.upload_path << "\n";
    // std::cout << "max_body = " << it_->second.serv_.max_body<< "\n";
    // std::cout << "upload: " << it_->second.requst.upload_state << std::endl;
    // std::cout << "====================\n";
    // std::cout << buffer << std::endl;
    // std::cout << "====================\n";
    g = 0;
    if (buffer.find("\r\n\r\n") != std::string::npos && f == 0)
    {
        parse_header(buffer);
        if (content_type.empty() || (content_length.empty() && transfer_encoding != "chunked"))
        {
            std::cout << "content type is empty " << content_type << std::endl;
            g = 1;
            buffer.clear();
            return true;
        }
        if (!extension_founded(content_type) && content_type.substr(0, 19) != "multipart/form-data")
        {
            std::cout << "content type: " << content_type << std::endl;
            g = 2;
            buffer.clear();
            return true;
        }
        if (extension_founded(content_type))
        {
            file = (generateUniqueFilename() + extension);
            outFile.open((it_->second.requst.upload_path + file).c_str());
        }
        else if (content_type.substr(0, 19) != "multipart/form-data")

            return true;
        buffer = buffer.substr(buffer.find("\r\n\r\n") + 4);
        if (transfer_encoding == "chunked")
        {
            chunked_len = 0;
            parse_hexa(buffer);
        }
        else if (transfer_encoding != "chunked" && g == 10)
        {
            std::cout << "$$$" << transfer_encoding << "$$$" << std::endl;
            g = 1;
            buffer.clear();
            return true;
        }
        else if (content_type.substr(0, 19) == "multipart/form-data")
        {
            sep = "--" + content_type.substr(30);
            content_type = content_type.substr(0, 19);
        }
        f = 1;
    }
    if (transfer_encoding == "chunked")
        return chunked(buffer, it_->second.serv_.max_body, it_->second.requst.upload_path);
    else if (content_type == "multipart/form-data")
        return boundary(buffer);
    else
        return binary(buffer, it_->second.serv_.max_body, it_->second.requst.upload_path);
    return false;
}

std::string post::parse_boundary_header(std::string buffer)
{
    std::string CT = "";
    if (buffer.find("Content-Type") != std::string::npos && buffer.find("\r\n\r\n") != std::string::npos) // not sure;
    {
        CT = buffer.substr(buffer.find("Content-Type"));
        CT = CT.substr(14);
        CT = CT.substr(0, CT.find("\r\n\r\n"));
    }
    return CT;
}

std::string post::cat_header(std::string buffer)
{
    return buffer.substr(buffer.find("\r\n\r\n") + 4);
}

int v = 0;
std::string CType = "";
std::vector<std::string> vec;

bool post::boundary(std::string buffer)
{
    /* ----------------------------261896924513075486597166
    Content-Disposition: form-data; name=""; filename="boundary.txt"
    Content-Type: text/plain \r\n\r\n*/
    concat += buffer;
    std::string file;
    while(1)
    {
        if (v == 0 && concat.find(sep) == 0)
        {
            if (concat.find("\r\n\r\n") != std::string::npos)
            {
                CType = parse_boundary_header(concat);
                concat = cat_header(concat);
                if (extension_founded(CType))
                {
                    file = generateUniqueFilename() + extension;
                    outFile.open(file.c_str());
                    vec.push_back(file);
                    v = 1;
                }
                else
                {
                    for (size_t i = 0; i < vec.size(); i++)
                        remove(vec.at(i).c_str());
                    outFile.close();
                    vec.clear();
                    concat.clear();
                    g = 2;
                    v = 0;
                    f = 0;
                    return true;
                }
            }
            else
                return false;
        }
        if(outFile.is_open() == true && (concat.find("\r\n" + sep) != std::string::npos)) 
        {
            outFile << concat.substr(0, concat.find("\r\n" + sep));
            outFile.close();
            concat = concat.substr(concat.find(sep));
            v = 0;
        }
        else if (outFile.is_open() == true)
        {
            if (concat.length() > sep.length())
            {
                outFile << concat.substr(0, concat.length() - sep.length());
                concat = concat.substr(concat.length() - sep.length());
            }
            return false;
        }
        if (concat == (sep + "--\r\n"))
        {
            std::cout << "done1.\n";
            concat.clear();
            outFile.close();
            v = 0;
            f = 0;
            return true;
        }
    }
    return false;
}

void post::parse_hexa(std::string &remain)
{
    ss << std::hex << remain.substr(0, remain.find("\r\n"));
    ss >> chunk_length;
    ss.str("");
    remain = remain.substr(remain.find("\r\n") + 2); // the remaining body after hexa\r\n. if after hexa is \r\n it means that "\r\n0\r\n\r\n".
}

bool post::chunked(std::string buffer, std::string max_body_size, std::string upload_path)
{
    if (outFile.is_open())
    {
        concat += buffer;
        if (concat.length() >= (chunk_length + 9))
        {
            outFile << concat.substr(0, chunk_length);
            chunked_len += concat.substr(0, chunk_length).length();
            concat = concat.substr(chunk_length + 2);
            parse_hexa(concat);
        }
        return is_end_of_chunk(max_body_size, upload_path);
    }
    else
        std::cerr << "Error opening file.\n";
    return false;
}

bool post::binary(std::string buffer, std::string max_body_size, std::string upload_path)
{
    if (outFile.is_open())
    {
        outFile << buffer;
        body_size += buffer.size();
        if (body_size > atoi(max_body_size.c_str()) || atoi(content_length.c_str()) > atoi(max_body_size.c_str()))
        {
            outFile.close();
            remove((upload_path + file).c_str());
            buffer.clear();
            body_size = 0;
            content_type.clear();
            f = 0; // header flag;
            g = 3; // request flag;
            return true;
        }
        else if (body_size > atoi(content_length.c_str()))
        {
            outFile.close();
            remove((upload_path + file).c_str());
            buffer.clear();
            body_size = 0;
            content_type.clear();
            f = 0; // header flag;
            g = 1; // request flag;
            return true;
        }
        else if (body_size == atoi(content_length.c_str()))
        {
            outFile.close();
            buffer.clear();
            content_type.clear();
            body_size = 0;
            f = 0; // header flag;
            g = 0; // request flag;
            return true;
        }
        // time out should be handled in multiplixing;
    }
    else
        std::cerr << "Error opening file.\n";
    return false;
}