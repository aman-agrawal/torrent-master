#include <iostream>
#include <stdlib.h>
#include <string>
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <string.h> 
#include <vector>

#include <fstream>

#include "mtorrent_file.h" 
#include "torrent_utils.h"

#define MAX 524288

using namespace std;

ofstream log_file;

void establish_connection_with_one_peer(vector<string> info, int peer_no, 
    mtorrent_file mt_file, string directory_name) {

    string seeder_ip, seeder_port;

    get_ip_port(info[2], seeder_ip, seeder_port);

    log_file << get_current_time() << " | " << " seeder ip : " << seeder_ip << " seeder port : " << seeder_port << endl;

    /**********************/

    struct sockaddr_in address; 
    int sock = 0, valread; 
    struct sockaddr_in server_address;

    char buffer[MAX] = {0}; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        log_file << get_current_time() << " | " << "error in calling socket" << endl;
        return; 
    } 
   
    log_file << get_current_time() << " | " << "socket called successfully" << endl;

    memset(&server_address, '0', sizeof(server_address)); 
   
    server_address.sin_family = AF_INET; 
    server_address.sin_port = htons(atoi(seeder_port.c_str()));
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, seeder_ip.c_str(), &server_address.sin_addr)<=0)  
    { 
        log_file << get_current_time() << " | " << "error in address specifications" << endl; 
        return; 
    } 

    log_file << get_current_time() << " | " << "inet_pton called successfully" << endl;
   
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) 
    { 
        log_file << get_current_time() << " | " << "error in calling connection" << endl;
        return; 
    }

    log_file << get_current_time() << " | " << "connect called successfully" << endl;

    /* 1. send file name */
    string info_to_share = "send " + mt_file.file_name;
    bzero(buffer, MAX);
    strcpy(buffer, info_to_share.c_str());
    send(sock , buffer , strlen(buffer) , 0 );
    log_file << get_current_time() << " | " << "1. sending : " << buffer << endl;

    /*2 dummy recieve */
    //memset(&buffer, 0, MAX); 
    bzero(buffer, MAX);
    read( sock , buffer, MAX);
    log_file << get_current_time() << " | " << "2. recieved : " << buffer << endl;

    /*3. send peer no */
    info_to_share = to_string(peer_no);
    bzero(buffer, MAX);
    strcpy(buffer, info_to_share.c_str());
    send(sock , buffer , strlen(buffer) , 0 );
    log_file << get_current_time() << " | " << "3. sending : " << buffer << endl;

    /*4. recieved size of data */
    int size_of_file;
    bzero(buffer, MAX);
    //memset(&buffer, 0, MAX); 
    read( sock , buffer, MAX);
    log_file << get_current_time() << " | " << "4. recieved : " << buffer << endl;
    //cout << "recieved size in buffer :  " << buffer << endl;
    size_of_file = atoi(buffer);
    //cout << "size_of_file : " << size_of_file;
    //cout << "strlen(buffer) : " << strlen(buffer) << endl;

    /*5. dummy send  */
    info_to_share = "okay";
    bzero(buffer, MAX);
    strcpy(buffer, info_to_share.c_str());
    send(sock , buffer , strlen(buffer) , 0 );
    log_file << get_current_time() << " | " << "5. sending : " << buffer << endl;

    /*6. recieved data */
    //memset(&buffer, 0, sizeof(buffer)); 
    bzero(buffer, MAX);
    read( sock , buffer, MAX);
    log_file << get_current_time() << " | " << "6. recieved : " << buffer << endl;
    
    cout << "File downloaded!" << endl;

    ofstream file_write;
    string file_name = file_name_from_path(mt_file.file_name);
    file_write.open( directory_name + "/" + file_name);
    
    file_write.write(buffer, size_of_file);
    file_write.close();
}

void establish_connection_with_peers(string command, mtorrent_file mt_file, 
    vector<string> seeders, string directory_name) {

    int peer_threads = 0;
    thread t[100];

    log_file << get_current_time() << " | " << "inside establish_connection_with_peers" << endl;

    log_file << get_current_time() << " | " << "size of seeders : " << seeders.size() << endl;

    for(int i=0; i < seeders.size() - 1; i++) {
        vector<string> info = get_tokens(seeders[i]);

        log_file << get_current_time() << " | " << "seeder : " << info[0] << " -- " << info[1] << " -- " << info[2] << endl;

        t[peer_threads] = thread(establish_connection_with_one_peer, info, i, mt_file, directory_name);        
        peer_threads++;

        break;
        
    }

    for(int i=0; i < peer_threads; i++) {
        t[i].join();
    }
}

vector<string> establish_connection_with_tracker(string command, mtorrent_file mt_file) {

    vector<string> seeders;

    log_file << get_current_time() << " | " << "inside establish_connection_with_tracker" << endl;

    struct sockaddr_in address; 
    int sock = 0, valread; 
    struct sockaddr_in server_address;

    string info_to_share; 
    
    char buffer[MAX] = {0}; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        log_file << get_current_time() << " | " << "error in calling socket" << endl;
        return seeders; 
    } 
   
    log_file << get_current_time() << " | " << "socket called successfully" << endl;

    memset(&server_address, '0', sizeof(server_address)); 
   
    server_address.sin_family = AF_INET; 
    server_address.sin_port = htons(atoi(mt_file.tracker_1_port.c_str()));
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, mt_file.tracker_1_ip.c_str(), &server_address.sin_addr)<=0)  
    { 
        log_file << get_current_time() << " | " << "error in address specifications" << endl; 
        return seeders; 
    } 

    log_file << get_current_time() << " | " << "inet_pton called successfully" << endl;
   
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) 
    { 
        log_file << get_current_time() << " | " << "error in calling connection" << endl;
        return seeders; 
    }

    log_file << get_current_time() << " | " << "connect called successfully" << endl;

    if(command == "share") {

        log_file << get_current_time() << " | " << "share : details sent to tracker " << endl;

        info_to_share = command + " "
                        + mt_file.file_name + " " 
                        + compute_sha_of_hash_string(mt_file.hash_string) + " "
                        + mt_file.client_ip + ":"
                        + mt_file.client_port;

        send(sock , info_to_share.c_str() , info_to_share.size() , 0 );

        cout << "File shared with trackers!" << endl;

        return seeders; 
    } else if(command == "get") {

        log_file << get_current_time() << " | " << "get : fetching details" << endl;

        info_to_share = command + " "
                        + mt_file.file_name + " ";

        log_file << get_current_time() << " | " << "get : info_to_share : " << info_to_share << endl;

        send(sock , info_to_share.c_str() , info_to_share.size() , 0 );

        log_file << get_current_time() << " | " << "get : details sent to tracker " << endl;

        read( sock , buffer, MAX); //contains whole line of seeder from seederlist

        log_file << get_current_time() << " | " << "get : " << buffer;
        log_file << get_current_time() << " | " << "get : details recieved from tracker " << endl;

        seeders = parse_seeder_list(buffer);

        return seeders;
    }
    
}

int process_request(mtorrent_file mt_file, char buffer[MAX]) {
    
    log_file << get_current_time() << " | " << "inside process_request" << endl;

    vector<string> tokens = get_tokens(buffer);

    if(strcmp(tokens[0].c_str(), "exit") == 0) {
            return -1;
    }

    string command = tokens[0];

    if(strcmp(command.c_str(), "send") == 0) {

    }
    else if(strcmp(command.c_str(), "share") == 0) {
        if(tokens.size() < 2) {
            cout << "Error : Please provide file path!" << endl;    
        } else if(tokens.size() < 3) {
            cout << "Error : Please provide mtorrent file name!" << endl;
        } else {
            log_file << get_current_time() << " | " << "Sharing" << endl;

            /*
                ## mtorrent file
                <tracker_1_ip>:<port>
                <tracker_2_ip>:<port>
                <filename>
                <filesize in bytes>
                <sha-1 concatenated hash string>

            */
            
        
            mtorrent_file mt_file_up = create_mtorrent_file(tokens[1], tokens[2], mt_file);

            establish_connection_with_tracker(command, mt_file_up);
        }
        
        return 0;
    
    } else if(strcmp(command.c_str(), "get") == 0) {
        
        if(tokens.size() < 2) {
            log_file << get_current_time() << " | " << "Error : Please provide mtorrent file path!" << endl;
        } else if(tokens.size() < 3) {
            log_file << get_current_time() << " | " << "Error : Please provide destination file path!" << endl;
        } else {
            log_file << get_current_time() << " | " << "get : Downloading" << endl;

            string mtorrent_file_path = tokens[1];

            log_file << get_current_time() << " | " << "get : create mtorrent object" << endl;

            mtorrent_file mt_file_up = create_mtorrent_object(mtorrent_file_path);

            log_file << get_current_time() << " | " << "get : object created" << endl;

            vector<string> seeders = establish_connection_with_tracker(command, mt_file_up);

            log_file << get_current_time() << " | " << "get : seeders recieved" << endl;

            log_file << get_current_time() << " | " << "get : start connecting with peers" << endl;

            establish_connection_with_peers(command, mt_file_up, seeders, tokens[2]);

            log_file << get_current_time() << " | " << "get : closing connection with peers" << endl;
        }

        return 0;
    } else if(strcmp(command.c_str(), "connect") == 0) {

        vector<string> seeders;
        establish_connection_with_peers(command, mt_file, seeders, "");

    }
}

void serve_client(int new_socket, struct sockaddr_in address, mtorrent_file mt_file) {
    char buffer[MAX] = {0};

    log_file << get_current_time() << " | " << "New client joined : " << new_socket << endl;

    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 

    /*1. recieve command */
    bzero(buffer, MAX);
    read( new_socket , buffer, MAX);
    log_file << get_current_time() << " | " << "1. recieved : " << buffer << endl;

    /*2. dummy send */
    string dummy = "okay";
    send(new_socket , dummy.c_str() , dummy.size() , 0 );
    log_file << get_current_time() << " | " << "2. seding : " << dummy << endl;

    vector<string> tokens = get_tokens(buffer);

    if(tokens[0] == "send") {
        
        ifstream file_read;
        log_file << get_current_time() << " | " << "prepare to send : " << tokens[1] << endl;
        
        file_read.open(tokens[1], ios::binary);

        /*3. recieve token no */
        bzero(buffer, MAX);
        read( new_socket , buffer, MAX);
        log_file << get_current_time() << " | " << "3. recieved : " << buffer << endl;

        tokens = get_tokens(buffer);

        char ch;
        long int counter = 0;

        char buzz[MAX];
        string s;
        bzero(buzz, MAX);
        
        

        while(file_read.get(ch)) {
            log_file << get_current_time() << " | " << "reading : " << counter << " : " << ch << endl;
            buzz[counter++] = ch;
        }

        /*4. send size of data */
        string size_to_share = to_string(counter);
        send(new_socket , size_to_share.c_str() , size_to_share.size() , 0 );
        log_file << get_current_time() << " | " << "4. sending : " << size_to_share << endl;

        log_file << get_current_time() << " | " << "sending : " << counter << " bytes" << endl;

        /*5. dummy recieve */
        bzero(buffer, MAX);
        read( new_socket , buffer, MAX);
        log_file << get_current_time() << " | " << "5. recieved : " << buffer << endl;

        cout << "Sending file..." << endl;
        /*6. send data */
        buzz[counter] = '\0';
        send(new_socket , buzz, counter, 0);
        log_file << get_current_time() << " | " << "6. sending : " << buzz << endl;

        cout << "File sent!" << endl;

        file_read.close();


    }
}

void client_as_server(mtorrent_file mt_file) {

    log_file << get_current_time() << " | " << "client as server : started" << endl;

    char buffer[MAX] = {0};

    int server_threads = 0;
    thread t[100];

    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        log_file << get_current_time() << " | " << "error in calling socket" << endl; 
        exit(EXIT_FAILURE); 
    } 
    log_file << get_current_time() << " | " << "client as server : success in calling socket" << endl; 

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) { 
        log_file << get_current_time() << " | " << "error in calling setsockopt" << endl; 
        exit(EXIT_FAILURE);
    } 
    log_file << get_current_time() << " | " << "client as server : success in calling setsockopt" << endl; 

    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons(atoi(mt_file.client_port.c_str())); 

    log_file << get_current_time() << " | " << "client as server : listening on port : " << mt_file.client_port << endl; 
    
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) {
        log_file << get_current_time() << " | " << "error in calling bind" << endl;  
        exit(EXIT_FAILURE); 
    } 
    log_file << get_current_time() << " | " << "client as server : success in calling bind" << endl;  

    if (listen(server_fd, 3) < 0) { 
        log_file << get_current_time() << " | " << "error in calling listen" << endl;   
        exit(EXIT_FAILURE); 
    }

    log_file << get_current_time() << " | " << "client as server : success in calling listen" << endl;   

    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen)) < 0) { 
            log_file << get_current_time() << " | " << " client as server : failure1 in create thread" << endl; 
        } else {
            t[server_threads] = thread(serve_client, new_socket, address, mt_file);        
            server_threads++;    

            log_file << get_current_time() << " | " << " client as server : success in create thread" << endl;   
              
        }
    }

    log_file << get_current_time() << " | " << " client as server : now join" << endl;   

    for(int i=0; i < server_threads; i++) {
        t[i].join();
    }
}

void local_client(mtorrent_file mt_file) {
    char buffer[MAX] = {0};

    while(1) {

        cout << ":";

        bzero(buffer, MAX);
        cin.getline(buffer, MAX);
        process_request(mt_file, buffer);
    }
}
int main(int argc, char *argv[]) {

    int main_threads = 0;
    thread t[100];

    if(argc < 5) {
        cout << "Too few arguements." << endl;
        log_file << get_current_time() << " | " << "Too few arguements." << endl;
        return 1;
    }

    /*
    Arguements in argv :

    1. client ip:port
    2. tracker 1 ip:port
    3. tracker 2 ip:port
    4. log file
    */

    string client_ip_port = argv[1];
    string tracker_1_ip_port = argv[2];
    string tracker_2_ip_port = argv[3];
    string log_file_path = argv[4];

    log_file.open(log_file_path);

    string client_ip, client_port;
    string tracker_1_ip, tracker_1_port;
    string tracker_2_ip, tracker_2_port;

    get_ip_port(client_ip_port, client_ip, client_port);
    get_ip_port(tracker_1_ip_port, tracker_1_ip, tracker_1_port);
    get_ip_port(tracker_2_ip_port, tracker_2_ip, tracker_2_port);

    mtorrent_file mt_file;

    mt_file.client_ip = client_ip;
    mt_file.client_port = client_port;
    mt_file.tracker_1_ip = tracker_1_ip;
    mt_file.tracker_1_port = tracker_1_port;
    mt_file.tracker_2_ip = tracker_2_ip;
    mt_file.tracker_2_port = tracker_2_port;

    log_file << get_current_time() << " | " << "Client started" << endl;
    
    //Launch a thread
    t[main_threads] = thread(client_as_server, mt_file);        
    main_threads++;
    
    //Launch a thread
    t[main_threads] = thread(local_client, mt_file);        
    main_threads++;

    for(int i=0; i < main_threads; i++) {
        t[i].join();
    }
    

    /**********************************************************/

    log_file.close();

    return 0; 
} 
