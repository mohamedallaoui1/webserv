/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayylaaba <ayylaaba@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/03 15:31:39 by ayylaaba          #+#    #+#             */
/*   Updated: 2024/02/21 12:59:11 by ayylaaba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"


int main(int ac, char **av)
{
    int             fd_sock, new_sock;
    long            rd_sock;
    sockaddr_in     address;
    server              parse;
    
    if (ac < 2)
        parse.print_err("argement error \n");

    parse.mange_file(av[1]);
    
    // std::cout << "\n Done" << std::endl;
    
    // std::vector<server*>::iterator b = parse.s.begin();
    // std::vector<server*>::iterator e = parse.s.end();
    // // std::cout << "\n\n SERVER_size =  " << parse.s.size() << "\n";
    size_t i = 0;
    size_t j = 0;

    while (i < parse.s.size()) 
    {
    //     // std::cout << "\n\nserver ----------------------------\n";
    //     std::map<std::string, std::string>::iterator  it;
    //     std::map<std::string, std::string>::iterator ite = parse.s[i]->cont.end();
    //     for (it = parse.s[i]->cont.begin(); it != ite; it++) 
    //     {
    //         if (!(*it).first.compare("listen"))
    //             // std::cout << (*it).first << " " << (*it).second << "\n";
    //         else if (!(*it).first.compare("server_name"))
    //             // std::cout << (*it).first << " " << (*it).second << "\n";
    //         else if ((*it).first == "root")
    //             // std::cout << (*it).first << " " << (*it).second << "\n";
    //         else if ((*it).first == "error_page")
    //             // std::cout << (*it).first << " " << (*it).second << "\n";
    //         else if ((*it).first == "host")
    //             // std::cout << (*it).first << " " << (*it).second << "\n";
    //         else if ((*it).first == "client_max_body_size")
    //             // std::cout << (*it).first << " " << (*it).second << "\n";
    //         else if ((*it).first == "index")
    //             // std::cout << (*it).first << " " << (*it).second << "\n";
    //     }
    //     // std::cout << "\n\n LOCATION_size =  " << parse.s[i]->l.size() << "\n";
    //     size_t j = 0;
        while (j < parse.s[i]->l.size())
        {
            // std::cout << "\n\nlocation ----------------------------\n";
            // std::cout << "=====> " << parse.s[i]->l[i]->allowed_methods[0] << " <====== \n";
            // std::cout << "=====> " << parse.s[i]->l[i]->allowed_methods[1] << " <====== \n";
            std::map<std::string, std::string>::iterator  it_b = parse.l[j]->cont_l.begin();
            std::map<std::string, std::string>::iterator it_e = parse.l[j]->cont_l.end();
            
            while (it_b != it_e) 
            {
                if ((*it_b).first == "location")
                    // std::cout << "hooooooo" << (*it_b).first << " " << (*it_b).second << "\n";
                // if ((*it_b).first == "root")
                //     // std::cout << (*it_b).first << " " << (*it_b).second << "\n";
                // if ((*it_b).first == "index")    
                //     // std::cout << (*it_b).first << " " << (*it_b).second << "\n";
                // if ((*it_b).first == "limit_except")
                //     // std::cout << (*it_b).first << " " << (*it_b).second << "\n";
                // if ((*it_b).first == "allow_methods")
                //     // std::cout << (*it_b).first << " " << (*it_b).second << "\n";
                // if ((*it_b).first == "autoindex")
                //     // std::cout << (*it_b).first << " " << (*it_b).second << "\n";
                // if ((*it_b).first == "upload")
                //     // std::cout << (*it_b).first << " " << (*it_b).second << "\n";
                it_b++;
            }
            j++;
        }
        i++;
    }
        
    /**************************************************/
    if ((fd_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
        parse.print_err("error connection\n");

    address.sin_family = AF_INET;
    address.sin_port = htons(8080);  // Corrected the port number format
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd_sock, (struct sockaddr*)&address, sizeof(address)) < 0)
        parse.print_err("error bind \n");

    if (listen(fd_sock, 10) < 0)
        parse.print_err("error listening \n");

    while (1)
    {
        printf("I am listening for a client...\n");

        if ((new_sock = accept(fd_sock, 0, 0)) < 0)  // Check for error in accept
            parse.print_err("client socket not created yet \n");

        char buffer[3000];
        rd_sock = read(new_sock, buffer, sizeof(buffer));  // Read from the accepted socket
        // // std::cout << "buffer = " << std::string(buffer, rd_sock) << std::endl;
        printf ("--> %s \n", buffer);
        char server_buffer[3000] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>Hello, World!</h1>";
        send(new_sock, server_buffer, strlen(server_buffer), 0);
        // // std::cout << server_buffer << "\n";  
        close(new_sock);  // Close the accepted socket, not the listening socket
    }

    // Note: The listening socket (fd_sock) is not closed here, as the server is designed to run indefinitely.
    return 0;
}
    // Initialize clientSockets array
    // for (int i = 0; i < MAX_CLIENTS; ++i) {
    //     clientSockets[i] = 0;
    // }

    // maxSockets = serverSocket;
    // while (true) 
    // {
    //     FD_ZERO(&readfds);
    //     FD_SET(serverSocket, &readfds);
    //     for (int i = 0; i < MAX_CLIENTS; ++i) 
    //     {
    //         if (clientSockets[i] > 0) 
    //             FD_SET(clientSockets[i], &readfds);            
    //     }
    //     // Use select to monitor sockets
    //     // std::cout << "7adi kinamss \n";
    //     activity = select(maxSockets + 1, &readfds, 0, 0, 0);
    //     if ((activity < 0) && (errno != EINTR)) 
    //         perror("Select error");
    //     // Check if there is a new connection
    //     if (FD_ISSET(serverSocket, &readfds)) 
    //     {
    //         if ((newSocket = accept(serverSocket, 0, 0)) < 0) {
    //             perror("Accept failed");
    //             exit(EXIT_FAILURE);
    //         }     
    //         // Add new socket to array of sockets

    //         for (int i = 0; i < MAX_CLIENTS; ++i) 
    //         {
    //             if (clientSockets[i] == 0) 
    //             {
    //                 //
    //                 clientSockets[i] = newSocket;
    //                 break;
    //             }
    //         }
    //         // Update maxSockets if needed
    //         if (newSocket > maxSockets) 
    //             maxSockets = newSocket;
    //         // std::cout << "New connection, socket fd is " << newSocket << ", IP is: "
    //                     << inet_ntoa(serverAddr.sin_addr) << ", port is: " << ntohs(serverAddr.sin_port) << std::endl;
    //     }
    //         // Check data from clients
    //     for (int i = 0; i < MAX_CLIENTS; ++i)     
    //     {
    //         int socketDesc = clientSockets[i];

    //         if (FD_ISSET(socketDesc, &readfds)) 
    //         {
    //             // std::cout << "deja t create had socketDes an it ready to read data from it => " << socketDesc << std::endl;   
    //             char buffer[BUFFER_SIZE];
    //             // std::cout << "Segv \n";
    //             int bytesRead = recv(socketDesc, buffer, BUFFER_SIZE, 0);
    //             std::string tmp = buffer;
    //             tmp.resize(bytesRead);
    //             respo = rq.read_request(tmp, parse);
    //             // std::cout << "after -> response -> = " << respo << " \n";
    //             if (bytesRead <= 0) 
    //             {
    //                 // Connection closed or error
    //                 close(socketDesc);
    //                 clientSockets[i] = 0;
    //             } 
    //             else
    //             {
    //                 // Echo back the data
    //                 send(socketDesc, respo.c_str(), respo.size(), 0);
    //                 sleep(4);
    //                 close(socketDesc);
    //             }
    //         }
    //     }
    // }

    // while (a < parse.s.size()) 
    // {
    //     while (k < parse.s[a]->l.size())
    //     {
    //         // std::cout << "\n\nlocation ----------------------------\n";
    //         // std::cout << "=====> " << parse.s[a]->l[k]->allowed_methods[0] << " <====== \n";
    //         // std::cout << "=====> " << parse.s[a]->l[k]->allowed_methods[1] << " <====== \n";


    //         std::map<std::string, std::string>::iterator  it_b = parse.l[k]->cont_l.begin();
    //         std::map<std::string, std::string>::iterator it_e = parse.l[k]->cont_l.end();

    //         // std::cout << "=====> " << (*it_b).first << " " << (*it_b).second <<   " <====== \n";
        
    //         while (it_b != it_e) 
    //         {
    //             if ((*it_b).first == "location")
    //                 // std::cout << "hooooooo " << (*it_b).first << " second == " << (*it_b).second << "\n";
    //             // if ((*it_b).first == "root")
    //             //     // std::cout << (*it_b).first << " " << (*it_b).second << "\n";
    //             // if ((*it_b).first == "index")    
    //             //     // std::cout << (*it_b).first << " " << (*it_b).second << "\n";
    //             // if ((*it_b).first == "limit_except")
    //             //     // std::cout << (*it_b).first << " " << (*it_b).second << "\n";
    //             // if ((*it_b).first == "allow_methods")
    //             //     // std::cout << (*it_b).first << " " << (*it_b).second << "\n";
    //             // if ((*it_b).first == "autoindex")
    //             //     // std::cout << (*it_b).first << " " << (*it_b).second << "\n";
    //             // if ((*it_b).first == "upload")
    //             //     // std::cout << (*it_b).first << " " << (*it_b).second << "\n";
    //             it_b++;
    //         }
    //         k++;
    //     }
    //     a++;
    // }
    // exit(1);
/*
    1- hawl tfham mzyan kifach kitra connection bin server and client (socket, bind, listen ...)

    2- hawl tfham mzyan select and its systems calls.

    3- akhir haja fham kifach multiplxing code works 

*/