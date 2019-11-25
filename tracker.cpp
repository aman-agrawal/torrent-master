#include <string.h> 
#include <thread>
#include <fstream>
#include <iostream>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 

#include "torrent_utils.h"
#include "mtorrent_file.h" 

using namespace std;

static int server_thread_no = 0;
thread server_threads[100];
ofstream log_file;

void create_thread(int new_socket, struct sockaddr_in address, mtorrent_file mt_file) {

    log_file << get_current_time() << " | " << "creating thread" << endl;

    char buffer[1024] = {0};

    log_file << get_current_time() << " | " << "New client joined : " << new_socket << endl;

    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 

    //char *msg = "SHA recieved"; 

    //send(new_socket , msg , strlen(msg) , 0 ); 

    //while(1) {
    bzero(buffer, 1024);
    read( new_socket , buffer, 1024);

    log_file << get_current_time() << " | " << "buffer : " << buffer << endl;

    vector<string> tokens = get_tokens(buffer);
    
    if(tokens.size() == 0) {
        log_file << get_current_time() << " | " << "no command found" << endl;
        return;
    }

    string command = tokens[0];
    log_file << get_current_time() << " | " << "command : " << command << endl;

    if(command == "exit") {
        log_file << get_current_time() << " | " << "exit command found" << endl;
        return;
    }

    if(command == "share") {

        log_file << get_current_time() << " | " << "share command found" << endl;
        log_file << get_current_time() << " | " << "writing in seeder_file : " << mt_file.seeder_file << endl; 

        ofstream seeder_file;
        seeder_file.open(mt_file.seeder_file, ios::app);
        for(int i=1; i < tokens.size(); i++) {
            seeder_file << tokens[i] << " ";
            log_file << get_current_time() << " | " << tokens[i] << " " << endl;  
        }
        
        seeder_file << endl;
        seeder_file.close();

        printf("%s\n",buffer );
        bzero(buffer, 1024); 

    } else if(command == "get") {

        string requested_file = tokens[1];

        log_file << get_current_time() << " | " << "get command found" << endl;

        ifstream file_read;
        file_read.open(mt_file.seeder_file);
        string info_to_share;

        while (file_read.peek() != EOF) {
            bzero(buffer, 1024);
            file_read.getline(buffer, 1024);
            vector<string> seeder = get_tokens(buffer);

            if(requested_file == seeder[0]) {
                info_to_share = info_to_share + buffer + "\n";
            }
        }
        
        send(new_socket , info_to_share.c_str() , info_to_share.size() , 0 ); 


    }
    
//}
    log_file << get_current_time() << " | " << "Connection closing : " << new_socket << endl;
}

int main(int argc, char * argv[]) 
{ 

    if(argc < 5) {
        cout << "Too few arguements." << endl;
        return 1;
    }

    string tracker_1_ip_port = argv[1];
    string tracker_2_ip_port = argv[2];
    string seeder_file = argv[3];
    string log_file_path = argv[4];

    log_file.open(log_file_path, ios::app);

    string tracker_1_ip, tracker_1_port;
    string tracker_2_ip, tracker_2_port;

    get_ip_port(tracker_1_ip_port, tracker_1_ip, tracker_1_port);
    get_ip_port(tracker_2_ip_port, tracker_2_ip, tracker_2_port);

    mtorrent_file mt_file;

    mt_file.tracker_1_ip = tracker_1_ip;
    mt_file.tracker_1_port = tracker_1_port;
    mt_file.tracker_2_ip = tracker_2_ip;
    mt_file.tracker_2_port = tracker_2_port;
    mt_file.seeder_file = seeder_file;

    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024] = {0}; 
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        log_file << get_current_time() << " | " << "error in calling socket" << endl; 
        exit(EXIT_FAILURE); 
    } 
    
    log_file << get_current_time() << " | " << "tracker : success in calling socket" << endl; 

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) { 
        log_file << get_current_time() << " | " << "error in calling setsockopt" << endl;
        exit(EXIT_FAILURE);
    } 

    log_file << get_current_time() << " | " << "tracker : success in calling setsockopt" << endl;

    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( atoi(mt_file.tracker_1_port.c_str()) ); 
    
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) {
        log_file << get_current_time() << " | " << "error in calling bind" << endl;  
        exit(EXIT_FAILURE); 
    } 

    log_file << get_current_time() << " | " << "tracker : success in calling bind" << endl;

    if (listen(server_fd, 3) < 0) { 
        log_file << get_current_time() << " | " << "error in calling listen" << endl; 
        exit(EXIT_FAILURE); 
    }

    log_file << get_current_time() << " | " << "success in calling listen" << endl;

    while(1) {

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen))<0) { 
            log_file << get_current_time() << " | " << "error in calling accept" << endl;   
            exit(EXIT_FAILURE); 
        }


        server_threads[server_thread_no] = thread(create_thread, new_socket, address, mt_file);        
        server_thread_no++;
    }

    for(int i=0; i<server_thread_no; i++) {
        server_threads[i].join();
    }
    
    log_file.close();
    return 0; 
} 
