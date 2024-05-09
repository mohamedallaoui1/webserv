#include "../headers/Client.hpp"

extern std::map<int, Client*>  fd_maps;

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
std::string concat = "";
int chunked_len = 0;

post::post()
{
}

post::post(const post &other)
{
    *this = other;
}

post &post::operator=(const post &other)
{
    (void)other;
    return *this;
}

post::~post()
{
}

bool post::is_end_of_chunk(std::string max_body_size, std::string upload_path, int fd)
{
    if (concat.find("\r\n0\r\n\r\n") != std::string::npos || chunk_length == 0)
    {
        outFile << concat.substr(0, concat.find("\r\n0\r\n\r\n"));
        chunked_len += concat.substr(0, concat.find("\r\n0\r\n\r\n")).length();
        outFile.close();
        outFile.clear();
        concat.clear();
        fd_maps[fd]->f = 0;
        if (chunked_len > atol(max_body_size.c_str()))
        {
            chunked_len = 0;
            g = 3;
            remove((upload_path + file).c_str());
            return true;
        }
        return true;
    }
    return false;
}

bool post::extension_founded(std::string contentType)
{
    extension = "";
    map m = read_file_extensions("fileExtensions");
    map::iterator it = m.find(contentType);
    if (it != m.end())
        extension = it->second;
    else
    {
        return false;
    }
    return true;
}

std::string sep = "";

bool post::post_method(std::string buffer, int fd)
{
    // std::cout << buffer << std::endl;
    // std::cout << "********* POST ************\n";
    // exit(12);
    std::map<int, Client*>::iterator   it_ = fd_maps.find(fd);
    g = 0;
    if (buffer.find("\r\n\r\n") != std::string::npos && fd_maps[fd]->f == 0)
    {
        parse_header(buffer.substr(0, buffer.find("\r\n\r\n") + 4));
        buffer = buffer.substr(buffer.find("\r\n\r\n") + 4);
        if (content_type.empty() || (content_length.empty() && transfer_encoding != "chunked"))
        {
            g = 1;
            return true;
        }
        if (!extension_founded(content_type) && content_type.substr(0, 19) != "multipart/form-data")
        {
            g = 2;
            return true;
        }
        if (extension_founded(content_type))
        {
            if (it_->second->is_cgi)
            {
                fd_maps[fd]->cgi_.file_in = generateCgiName();
                outFile.open(("/tmp/" + fd_maps[fd]->cgi_.file_in).c_str());
                if (!outFile.is_open())
                {
                    g = 5;
                    return true;
                }
            }
            else
            {
                file = generateUniqueFilename() + extension;
                outFile.open((it_->second->requst.upload_path + file).c_str());
                if (!outFile.is_open())
                {
                    g = 5;
                    return true;
                }
            }
        }
        else if (it_->second->is_cgi && content_type.substr(0, 19) == "multipart/form-data")
        {
            fd_maps[fd]->cgi_.file_in = generateCgiName();
            outFile.open(("/tmp/" + fd_maps[fd]->cgi_.file_in).c_str());
            if (!outFile.is_open())
            {
                g = 5;
                return true;
            }
        }
        else if (content_type.substr(0, 19) != "multipart/form-data")
            return true;
        if (transfer_encoding == "chunked" && content_type.substr(0, 19) == "multipart/form-data")
        {
            g = 4;
            return true;
        }
        if (transfer_encoding == "chunked")
        {
            chunked_len = 0;
            parse_hexa(buffer);
            if (g == 1)
            {
                outFile.close();
                chunk_length = 0;
                return true;
            }
        }
        else if (transfer_encoding != "chunked" && g == 10)
        {
            g = 1;
            buffer.clear();
            return true;
        }
        else if (content_type.substr(0, 19) == "multipart/form-data")
        {
            sep = "--" + content_type.substr(30);
            content_type = content_type.substr(0, 19);
        }
        fd_maps[fd]->f = 1;
        std::cout << "f: " << fd_maps[fd]->f << std::endl;
    }
    if (transfer_encoding == "chunked")
        return chunked(buffer, (*fd_maps[fd]->requst.it)->max_body, it_->second->requst.upload_path, fd);
    else if (content_type == "multipart/form-data" && !it_->second->is_cgi)
        return boundary(buffer, (*fd_maps[fd]->requst.it)->max_body, it_->second->requst.upload_path, fd);
    else
    {
        if (it_->second->is_cgi && content_type == "multipart/form-data")
        {
            return boundary_CGI(buffer, (*fd_maps[fd]->requst.it)->max_body, fd);
        }
        else
            return binary(buffer, (*fd_maps[fd]->requst.it)->max_body, it_->second->requst.upload_path, fd);
    }
    return false;
}

std::string post::parse_boundary_header(std::string buffer)
{
    std::string CT = "";
    if (buffer.find("Content-Type") != std::string::npos && buffer.find("\r\n\r\n") != std::string::npos)
    {
        CT = buffer.substr(buffer.find("Content-Type"));
        CT = CT.substr(14);
        CT = CT.substr(0, CT.find("\r\n\r\n"));
    }
    return CT;
}

std::string get_name(std::string buffer)
{
    std::string name = "";
    if (buffer.find("name") != std::string::npos && buffer.find("\r\n\r\n") != std::string::npos)
    {
        name = buffer.substr(buffer.find("name"));
        name = name.substr(6);
        name = name.substr(0, name.find("\""));
    }
    return name;
}

bool post::nameExistsInVector(std::vector<std::string> vec, std::string target)
{
    return std::find(vec.begin(), vec.end(), target) != vec.end();
}

std::string post::cat_header(std::string buffer)
{
    return buffer.substr(buffer.find("\r\n\r\n") + 4);
}

int v = 0;
std::string CType = "";
std::string name = "";
std::vector<std::string> vec;
int suffix = 0;
int len = 0;

bool post::boundary(std::string buffer, std::string max_body_size, std::string upload_path, int fd)
{
    /* ----------------------------261896924513075486597166
    Content-Disposition: form-data; name=""; filename="boundary.txt"
    Content-Type: text/plain \r\n\r\n*/
    concat += buffer;
    len += buffer.length();
    std::string file;
    while (1)
    {
        if (v == 0 && concat.find(sep) == 0)
        {
            if (concat.find("\r\n\r\n") != std::string::npos)
            {
                CType = parse_boundary_header(concat.substr(0, concat.find("\r\n\r\n") + 4));
                name = get_name(concat.substr(0, concat.find("\r\n\r\n") + 4));
                if (extension_founded(CType) && !name.empty())
                {
                    file = name + generateUniqueSuffix() + extension;
                    outFile.open((upload_path + file).c_str());
                    if (!outFile.is_open())
                    {
                        for (size_t i = 0; i < vec.size(); i++)
                            remove(vec.at(i).c_str());
                        outFile.close();
                        vec.clear();
                        concat.clear();
                        len = 0;
                        fd_maps[fd]->f = 0; // header flag;
                        v = 0;
                        g = 5;
                        return true;
                    }
                    vec.push_back(upload_path + file);
                    v = 1;
                }
                else if (extension_founded(CType) && name.empty())
                {
                    file = generateUniqueFilename() + extension;
                    outFile.open((upload_path + file).c_str());
                    if (!outFile.is_open())
                    {
                        for (size_t i = 0; i < vec.size(); i++)
                            remove(vec.at(i).c_str());
                        outFile.close();
                        vec.clear();
                        concat.clear();
                        len = 0;
                        fd_maps[fd]->f = 0; // header flag;
                        v = 0;
                        g = 5;
                        return true;
                    }
                    vec.push_back(upload_path + file);
                    v = 1;
                }
                else if (concat.substr(0, concat.find("\r\n\r\n") + 4).find("filename") == std::string::npos && !name.empty())
                {
                    file = name + generateUniqueSuffix() + ".txt";
                    outFile.open((upload_path + file).c_str());
                    if (!outFile.is_open())
                    {
                        for (size_t i = 0; i < vec.size(); i++)
                            remove(vec.at(i).c_str());
                        outFile.close();
                        vec.clear();
                        concat.clear();
                        len = 0;
                        fd_maps[fd]->f = 0; // header flag;
                        v = 0;
                        g = 5;
                        return true;
                    }
                    vec.push_back(upload_path + file);
                    v = 1;
                }
                else if (concat.substr(0, concat.find("\r\n\r\n") + 4).find("filename") == std::string::npos && name.empty())
                {
                    file = generateUniqueFilename() + ".txt";
                    outFile.open((upload_path + file).c_str());
                    if (!outFile.is_open())
                    {
                        for (size_t i = 0; i < vec.size(); i++)
                            remove(vec.at(i).c_str());
                        outFile.close();
                        vec.clear();
                        concat.clear();
                        len = 0;
                        fd_maps[fd]->f = 0; // header flag;
                        v = 0;
                        g = 5;
                        return true;
                    }
                    vec.push_back(upload_path + file);
                    v = 1;
                }
                else if (concat.substr(0, concat.find("\r\n\r\n") + 4).find("filename") != std::string::npos && concat.substr(0, concat.find("\r\n\r\n") + 4).find("Content-Type") == std::string::npos && !name.empty())
                {
                    file = name + generateUniqueSuffix() + ".txt";
                    outFile.open((upload_path + file).c_str());
                    if (!outFile.is_open())
                    {
                        for (size_t i = 0; i < vec.size(); i++)
                            remove(vec.at(i).c_str());
                        outFile.close();
                        vec.clear();
                        concat.clear();
                        len = 0;
                        fd_maps[fd]->f = 0; // header flag;
                        v = 0;
                        g = 5;
                        return true;
                    }
                    vec.push_back(upload_path + file);
                    v = 1;
                }
                else if (concat.substr(0, concat.find("\r\n\r\n") + 4).find("filename") != std::string::npos && concat.substr(0, concat.find("\r\n\r\n") + 4).find("Content-Type") == std::string::npos && name.empty())
                {
                    file = generateUniqueFilename() + ".txt";
                    outFile.open((upload_path + file).c_str());
                    if (!outFile.is_open())
                    {
                        for (size_t i = 0; i < vec.size(); i++)
                            remove(vec.at(i).c_str());
                        outFile.close();
                        vec.clear();
                        concat.clear();
                        len = 0;
                        fd_maps[fd]->f = 0; // header flag;
                        v = 0;
                        g = 5;
                        return true;
                    }
                    vec.push_back(upload_path + file);
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
                    fd_maps[fd]->f = 0;
                    len = 0;
                    return true;
                }
                concat = cat_header(concat);
            }
            else
                return false;
        }
        if (outFile.is_open() == true && (concat.find("\r\n" + sep) != std::string::npos))
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
                if (len > atol(max_body_size.c_str()))
                {
                    for (size_t i = 0; i < vec.size(); i++)
                        remove(vec.at(i).c_str());
                    outFile.close();
                    vec.clear();
                    concat.clear();
                    len = 0;
                    fd_maps[fd]->f = 0; // header flag;
                    v = 0;
                    g = 3; // request flag;
                    return true;
                }
                concat = concat.substr(concat.length() - sep.length());
            }
            return false;
        }
        if (concat == (sep + "--\r\n") && (long long)len == atol(content_length.c_str()))
        {
            concat.clear();
            outFile.close();
            vec.clear();
            v = 0;
            fd_maps[fd]->f = 0;
            len = 0;
            return true;
        }
        else if ((long long)len > atol(content_length.c_str()))
        {
            for (size_t i = 0; i < vec.size(); i++)
                remove(vec.at(i).c_str());
            outFile.close();
            vec.clear();
            concat.clear();
            len = 0;
            fd_maps[fd]->f = 0;
            v = 0;
            g = 1; // request flag;
            return true;
        }
        else if ((long long)len > atol(max_body_size.c_str()))
        {
            for (size_t i = 0; i < vec.size(); i++)
                remove(vec.at(i).c_str());
            outFile.close();
            vec.clear();
            concat.clear();
            len = 0;
            fd_maps[fd]->f = 0;
            v = 0;
            g = 3; // request flag;
            return true;
        }
    }
    return false;
}

void post::parse_hexa(std::string &remain)
{
    if (remain.find("\r\n") != std::string::npos)
    {
        ss << std::hex << remain.substr(0, remain.find("\r\n"));
        ss >> chunk_length;
        ss.str("");
    }
    else if (remain.find("\r\n") != std::string::npos)
    {
        g = 1;
    }
    if (remain.find("\r\n") + 2 != std::string::npos)
        remain = remain.substr(remain.find("\r\n") + 2); // the remaining body after hexa\r\n. if after hexa is \r\n it means that "\r\n0\r\n\r\n".
}

bool post::chunked(std::string buffer, std::string max_body_size, std::string upload_path, int fd)
{
    if (outFile.is_open())
    {
        concat += buffer;
        if (concat.length() >= (chunk_length + 9))
        {
            outFile << concat.substr(0, chunk_length);
            chunked_len += concat.substr(0, chunk_length).length();
            concat = concat.substr(chunk_length + 2); //skip \r\n after the body;
            parse_hexa(concat);
            if (g == 1)
            {
                outFile.close();
                outFile.clear();
                concat.clear();
                chunk_length = 0;
                fd_maps[fd]->f = 0;
                return true;
            }
        }
        return is_end_of_chunk(max_body_size, upload_path, fd_maps[fd]->f);
    }
    else
    {
        outFile.close();
        outFile.clear();
        concat.clear();
        chunk_length = 0;
        fd_maps[fd]->f = 0;
        g = 5;
        return true;
    }
    return false;
}

bool post::binary(std::string buffer, std::string max_body_size, std::string upload_path, int fd)
{
    if (outFile.is_open())
    {
        outFile << buffer;
        body_size += buffer.size();
        if (body_size > atol(max_body_size.c_str()))
        {
            outFile.close();
            remove((upload_path + file).c_str());
            buffer.clear();
            body_size = 0;
            fd_maps[fd]->f = 0;
            g = 3; // request flag;
            return true;
        }
        else if (body_size > atol(content_length.c_str()))
        {
            outFile.close();
            remove((upload_path + file).c_str());
            buffer.clear();
            body_size = 0;
            fd_maps[fd]->f = 0;
            g = 1; // request flag;
            return true;
        }
        else if (body_size == atol(content_length.c_str()))
        {
            outFile.close();
            buffer.clear();
            body_size = 0;
            fd_maps[fd]->f = 0;
            g = 0; // request flag;
            return true;
        }
    }
    else
    {
        outFile.close();
        buffer.clear();
        body_size = 0;
        fd_maps[fd]->f = 0;
        g = 5;
        return true;
    }
    return false;
}

bool post::boundary_CGI(std::string buffer, std::string max_body_size, int fd)
{
    concat += buffer;
    if (outFile.is_open() == true)
    {
        if (concat.find(sep + "--\r\n") != std::string::npos)
        {
            outFile << concat.substr(0, concat.find(sep + "--\r\n"));
            concat = concat.substr(concat.find(sep + "--\r\n"));
        }
        else if (concat.find(sep + "--\r\n") == std::string::npos)
        {
            outFile << concat.substr(0, concat.length() - sep.length());
            len += concat.substr(0, concat.length() - sep.length()).length();
            if (len > atol(max_body_size.c_str()))
            {
                for (size_t i = 0; i < vec.size(); i++)
                    remove(vec.at(i).c_str());
                outFile.close();
                vec.clear();
                concat.clear();
                CType.clear();
                fd_maps[fd]->f = 0;
                v = 0;
                g = 3; // request flag;
                return true;
            }
            concat = concat.substr(concat.length() - sep.length());
        }
        if (concat == (sep + "--\r\n"))
        {
            concat.clear();
            outFile.close();
            CType.clear();
            vec.clear();
            v = 0;
            fd_maps[fd]->f = 0;
            return true;
        }
    }
    else
    {
        concat.clear();
        outFile.close();
        CType.clear();
        vec.clear();
        v = 0;
        fd_maps[fd]->f = 0;
        g = 5;
        return true;
    }
    return false;
}