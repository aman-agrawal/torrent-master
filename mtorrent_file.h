#include<bits/stdc++.h>

using namespace std;


#ifndef INC_MTORRENT_FILE_H
#define INC_MTORRENT_FILE_H

class mtorrent_file {

public:
	string client_ip;

	string client_port;

	string tracker_1_ip;

	string tracker_1_port;

	string tracker_2_ip;

	string tracker_2_port;

	string file_name;

	unsigned long int file_size;

	string hash_string;

	string seeder_file;

	string log_file;

	string command;


	mtorrent_file();

	void show();
};

#endif