#include "../post.hpp"

void post::print_keyVal(map m)
{
    map::iterator it = m.begin();
    while (it != m.end())
    {
        std::cout << it->first << ":" << it->second << std::endl;
        it++;
    }
    std::cout << "\n";
}

std::string post::generateUniqueFilename()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    std::ostringstream filename_stream;
    filename_stream << "outfile_" << tv.tv_sec << "-" << tv.tv_usec;

    return filename_stream.str();
}