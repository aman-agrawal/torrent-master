#ifndef INC_TORRENT_UTILS_H
#define INC_TORRENT_UTILS_H

#include <iostream>
#include <string>
#include "mtorrent_file.h"

using namespace std;

vector<string> get_tokens(char buffer[1024]);

vector<string> get_tokens(string buffer);

vector<string> parse_seeder_list(char buffer[1024]);

string get_current_time();

void get_ip_port(string ip_port, string &ip, string &port);

mtorrent_file create_mtorrent_object(string mt_file_path);

mtorrent_file create_mtorrent_file(string original_file, string mt_file_name, mtorrent_file mt_file);

string compute_sha_of_hash_string(string hash_string);

string file_name_from_path(string path);

#endif