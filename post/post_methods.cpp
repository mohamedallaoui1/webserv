#include "../post.hpp"

/*-- My Global variables --*/

std::string fileName;
std::ofstream outFile;
std::string extension;

// for parsing header;
std::string contentType;
std::string content_length;
std::string transfer_encoding;

// for binary;
ssize_t body_size = 0;

// for chunked;
size_t chunk_length = 0;
std::stringstream ss;
std::string hexa;
std::string concat;
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

bool post::is_end_of_chunk()
{
    if (concat.find("\r\n0\r\n\r\n") != std::string::npos || chunk_length == 0)
    {
        outFile << concat.substr(0, concat.find("\r\n0\r\n\r\n"));
        outFile.close();
        outFile.clear();
        concat.clear();
        f = 0;
        return true;
    }
    return false;
}

bool post::extension_founded(std::string contentType)
{
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

bool post::post_method(std::string buffer)
{
    if (buffer.find("\r\n\r\n") != std::string::npos && f == 0)
    {
        parse_header(buffer, contentType, content_length, transfer_encoding);
        if (extension_founded(contentType))
            outFile.open((generateUniqueFilename() + extension).c_str());
        else if (contentType.substr(0, 19) != "multipart/form-data")
            return true;
        buffer = buffer.substr(buffer.find("\r\n\r\n") + 4);
        if (transfer_encoding == "chunked")
            parse_hexa(buffer);
        else if (contentType.substr(0, 19) == "multipart/form-data")
        {
            sep = "--" + contentType.substr(30);
            contentType = contentType.substr(0, 19);
        }
        f = 1;
    }
    if (transfer_encoding == "chunked")
        return chunked(buffer);
    else if (contentType == "multipart/form-data")
        return boundary(buffer);
    else
        return binary(buffer);
    return false;
}

std::string post::parse_boundary_header(std::string buffer)
{
    std::string CT = "";
    if (buffer.find("Content-Type") != std::string::npos && buffer.length() > 87) // not sure;
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

bool post::boundary(std::string buffer)
{
    /* ----------------------------108074513576787105840635
    Content-Disposition: form-data; name=""; filename="boundary.txt"
    Content-Type: text/plain */
    concat += buffer;
    while(concat.find(sep) != std::string::npos)
    {
        if (v == 0)
        {
            // std::cout << "here\n";
            CType = parse_boundary_header(concat);
            concat = cat_header(concat);
            if (extension_founded(CType))
            {
                outFile.open((generateUniqueFilename() + extension).c_str());
            }
            else
            {
                std::cerr << "extension not founded!\n";
                return false;
            }
            v = 1;
        }
        if(outFile.is_open() && concat.find(sep) != std::string::npos)
        {
            outFile << concat.substr(0, concat.find(sep) - 2); // -2 of \r\n;
            concat = concat.substr(concat.find(sep));
            outFile.close();
            outFile.clear();
            v = 0;
        }
        if (concat == (sep + "--\r\n"))
        {
            std::cout << "done1.\n";
            concat.clear();
            outFile.close();
            outFile.clear();
            v = 0;
            f = 0;
            return true;
        }
    }
    if (v == 0)
    {
        // std::cout << "concat =====>" << concat << std::endl;
        CType = parse_boundary_header(concat);
        concat = cat_header(concat);
        if (extension_founded(CType) == true)
        {
            outFile.open((generateUniqueFilename() + extension).c_str());
        }
        else
        {
            std::cerr << "414 unsupported media type\n";
        }
        v = 1;
    }
    if(outFile.is_open())
    {
        outFile << concat;
        if (concat == (sep + "--\r\n"))
        {
            concat.clear();
            outFile.close();
            outFile.clear();
            f = 0;
            v = 0;
            std::cout << "done2.\n";
            return true;
        }
        concat.clear();
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

bool post::chunked(std::string buffer)
{
    if (outFile.is_open())
    {
        concat += buffer;
        if (concat.length() >= (chunk_length + 9))
        {
            outFile << concat.substr(0, chunk_length);
            concat = concat.substr(chunk_length + 2);
            parse_hexa(concat);
        }
        return is_end_of_chunk();
    }
    else
        std::cerr << "Error opening file.\n";
    return false;
}

bool post::binary(std::string buffer)
{
    if (outFile.is_open())
    {
        outFile << buffer;
        body_size += buffer.size();
        if (body_size == atoi(content_length.c_str()))
        {
            outFile.close();
            buffer.clear();
            body_size = 0;
            f = 0;
            return true;
        }
        else if (atoi(content_length.c_str()) < body_size)
        {
            outFile.close();
            remove((generateUniqueFilename() + extension).c_str());
            buffer.clear();
            body_size = 0;
            f = 0;
            std::cerr << "ERROR: bad request.\n";
            return true;
        }
    }
    else
        std::cerr << "Error opening file.\n";
    return false;
}