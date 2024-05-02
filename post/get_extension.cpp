#include "../Client.hpp"

map post::read_file_extensions(const char *filename)
{
    map extensions;
    std::ifstream file(filename);
    std::string line;
    std::string key;
    std::string value;
    if (!file.is_open())
    {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return extensions;
    }
    while (getline(file, line)) 
    {
        key = line.substr(0, line.find(":"));
        value = line.substr(line.find(":") + 1);
        extensions[key] = value;
    }
    file.close();
    return extensions;
}