#include <bits/stdc++.h>

#include "mtorrent_file.h"

using namespace std;

mtorrent_file::mtorrent_file() {
	tracker_1_ip = "";
	tracker_1_port = "";
	tracker_2_ip = "";
	tracker_2_port = "";
	
	file_name = "";

	file_size = 0;

	hash_string = "";

	seeder_file = "";

	log_file = "";

	command = "";

}

void mtorrent_file::show() {
	cout << "tracker_1 ip port : " << tracker_1_ip << " : " << tracker_1_port << endl;
	cout << "tracker_2 ip port : " << tracker_2_ip << " : " << tracker_2_port << endl;
	cout << "file_name file_size : " << file_name << " : " << file_size << endl;
}