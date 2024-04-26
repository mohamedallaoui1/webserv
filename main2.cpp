# include <iostream>
# include <unistd.h>
# include <fstream>

int main() {
    
    if (access("/tmp/12457s8", F_OK) < 0) {
        std::cerr << "makayaaanch barcaaaa\n";
 
    }
   
}