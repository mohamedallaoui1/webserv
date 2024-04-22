#include "../post.hpp"

void post::PutBodyInFile(std::string to_join, std::string extension)
{
    std::string header = to_join;
    std::string body;
    size_t pos = header.find("\r\n\r\n");
    if(pos != std::string::npos) // means found;
    {
        body = header.substr(pos + 4);
        std::string filename = generateUniqueFilename();
        std::ofstream outFile((filename + extension).c_str());
        if (outFile.is_open())
        {
            outFile << body;
            outFile.close();
        } 
        else
            std::cerr << "Error: Unable to open file for writing." << std::endl;
    }
    else
        std::cerr << "Error: Body not found in buffer." << std::endl;
}