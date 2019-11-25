#include <iostream>
#include <ctime>
#include <string.h>
#include <openssl/sha.h>
#include <fstream>
#include <sys/stat.h>

#include "mtorrent_file.h"
#include "torrent_utils.h"

#define MAX 524288	//512Bytes
/*#define MAX 10*/

using namespace std;

vector<string> get_tokens(char buffer[1024]) {

    vector<string> tokens;
    string temp;

    char ch;

    for(int i =0; i < strlen(buffer); i++) {
        ch = buffer[i];

        if(ch == ' ' || ch == '\n' || ch == '\0') {
            tokens.push_back(temp);
            temp = "";
        } else {
            temp = temp + ch;
        }

    }
    tokens.push_back(temp);

    return tokens;
}

vector<string> get_tokens(string buffer) {

    vector<string> tokens;
    string temp;

    char ch;

    for(int i =0; i < buffer.size(); i++) {
        ch = buffer[i];

        if(ch == ' ' || ch == '\n' || ch == '\0') {
            tokens.push_back(temp);
            temp = "";
        } else {
            temp = temp + ch;
        }

    }
    tokens.push_back(temp);

    return tokens;
}

vector<string> parse_seeder_list(char buffer[1024]) {

    vector<string> tokens;
    string temp;

    char ch;

    for(int i =0; i < strlen(buffer); i++) {
        ch = buffer[i];

        if(ch == '\n' || ch == '\0') {
            tokens.push_back(temp);
            temp = "";
        } else {
            temp = temp + ch;
        }

    }
    tokens.push_back(temp);

    return tokens;
}

void get_ip_port(string ip_port, string &ip, string &port) {

    int i=0;
    for(i=0;i<ip_port.size();i++) {
        if(ip_port[i] == ':')
            break;
    }
    ip = ip_port.substr(0, i);
    port = ip_port.substr(i+1, ip_port.size()-i-1);
}

string get_current_time(){

	time_t now = time(0);
	string time;

	tm *ltm = localtime(&now);

	
	time = time + to_string(ltm->tm_mday) + "/" + to_string(1 + ltm->tm_mon) 
			+ "/" + to_string(1900 + ltm->tm_year)
			+ " | " + to_string(ltm->tm_hour) + ":" + to_string(ltm->tm_min) 
			+ ":" + to_string(ltm->tm_sec);

	return time;
}

mtorrent_file create_mtorrent_object(string mt_file_path) {
	ifstream file_read;
	file_read.open(mt_file_path);

	char buffer[1024];

	mtorrent_file mt_file;

	string tracker_1_ip, tracker_1_port;
    string tracker_2_ip, tracker_2_port;

	bzero(buffer, 1024);
    file_read.getline(buffer, 1024);
    string tracker_url_1 = buffer;
    get_ip_port(tracker_url_1, tracker_1_ip, tracker_1_port);
    mt_file.tracker_1_ip = tracker_1_ip;
    mt_file.tracker_1_port = tracker_1_port;

    bzero(buffer, 1024);
    file_read.getline(buffer, 1024);
    string tracker_url_2 = buffer;
    get_ip_port(tracker_url_2, tracker_2_ip, tracker_2_port);
    mt_file.tracker_2_ip = tracker_2_ip;
    mt_file.tracker_2_port = tracker_2_port;

    bzero(buffer, 1024);
    file_read.getline(buffer, 1024);
    mt_file.file_name = buffer;

    bzero(buffer, 1024);
    file_read.getline(buffer, 1024);
    mt_file.file_size = atoi(buffer);

    bzero(buffer, 1024);
    file_read.getline(buffer, 1024);
    mt_file.hash_string = buffer;

    //mt_file.show();

    return mt_file;
}

string compute_sha_of_hash_string(string hash_string) {

	char input[MAX];

	strcpy(input, hash_string.c_str());

    char buffer[SHA_DIGEST_LENGTH*2];
    unsigned char output[SHA_DIGEST_LENGTH];
    
    memset(buffer, 0x0, SHA_DIGEST_LENGTH*2);
    memset(output, 0x0, SHA_DIGEST_LENGTH);

    SHA1((unsigned char *) input, strlen(input), output);

    for (int i=0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf((char*)&(buffer[i*2]), "%02x", output[i]); 
    }

    string sha(buffer, 20);

    return sha;
}

string get_sha_from_mtorrent_file(string mt_file) {

	char buffer[1024] = {0}; 

	ifstream file_read;

	file_read.open(mt_file);

	for(int i=0;i<5;i++) {
		bzero(buffer, 1024);
    	cin.getline(buffer, 1024);
	}
	string sha(buffer);
	return sha;
}

string file_name_from_path(string path) {

	string file_name;
	int i;
	for(i = path.length() - 1; i >= 0; i--) {
		//cout << path[i] << endl;
		if(path[i] == '/') {
			break;
		}
	}

	file_name = path.substr(i+1, path.length() - i - 1);

	for(i = file_name.length() - 1; i >= 0; i--) {
		if(file_name[i] == '.') {
			break;
		}
	}

	file_name = file_name.substr(0, i);

	return file_name;
}

string get_file_name_from_path(string path) {

	string file_name;
	int i;
	for(i = path.length() - 1; i >= 0; i--) {
		//cout << path[i] << endl;
		if(path[i] == '/') {
			break;
		}
	}

	file_name = path.substr(i+1, path.length() - i - 1);

	for(i = file_name.length() - 1; i >= 0; i--) {
		if(file_name[i] == '.') {
			break;
		}
	}

	file_name = file_name.substr(0, i);

	return file_name;
}

mtorrent_file create_mtorrent_file(string original_file, string mt_file_name, mtorrent_file mt_file) {

	
	ifstream file_read;
	ofstream file_write;

	file_read.open(original_file, ios::binary);
	file_write.open("temp.txt", ios::trunc);
	file_write.close();

	char input[MAX];
	char ch;
	long int counter = 0;

	char buffer[SHA_DIGEST_LENGTH*2];
	unsigned char output[SHA_DIGEST_LENGTH];

	while(file_read >> ch) {

		if(counter == MAX) {

			memset(buffer, 0x0, SHA_DIGEST_LENGTH*2);
       		memset(output, 0x0, SHA_DIGEST_LENGTH);

			SHA1((unsigned char *) input, counter, output);

			for (int i=0; i < SHA_DIGEST_LENGTH; i++) {
        		sprintf((char*)&(buffer[i*2]), "%02x", output[i]); 
    		}

    		file_write.open("temp.txt", ios::app);
			file_write.write(buffer, 20);
			file_write.close();

    		//cout << endl;
		    int i;
		    for (i = 0; i < 20; i++) {
		        printf("%02x", output[i]);
		    }

			counter = 0;

			input[counter] = ch;

			counter++;

		} else {
			input[counter] = ch;
			counter++;
		}
	}

	if(counter != 0) {

		memset(buffer, 0x0, SHA_DIGEST_LENGTH*2);
       	memset(output, 0x0, SHA_DIGEST_LENGTH);

       	if(input[counter-1] == '\n') {
       		//cout << "Putting zero";
       		input[counter-1] = '\0';
       	}

		SHA1((unsigned char *) input, counter, output);

		for (int i=0; i < SHA_DIGEST_LENGTH; i++) {
        	sprintf((char*)&(buffer[i*2]), "%02x", output[i]); 
    	}

    	file_write.open("temp.txt", ios::app);
    	file_write.write(buffer, 20);
    	file_write.close();

    	/*cout << endl;
	    int i;
	    for (i = 0; i < 20; i++) {
	        printf("%02x", output[i]);
	    }*/
	}

	file_read.close();

	

/**********************************************************
	Finally read the temp file and create its SHA
***********************************************************/

	//cout << endl << "Final :" << endl;

	file_read.open("temp.txt");

	counter = 0;

	memset(input, 0x0, MAX);

	while(file_read >> ch) {
		
		input[counter++] = ch;
	}

	mt_file.file_name = original_file;

	struct stat results;

    stat(original_file.c_str(), &results);
    mt_file.file_size = results.st_size;

    mt_file.hash_string = input;

    

	//string file_name = get_file_name_from_path(original_file);
	//string m_torrent_file = file_name + ".mtorrent";
	file_write.open(mt_file_name);
    
    file_write << mt_file.tracker_1_ip << ":" << mt_file.tracker_1_port << endl;
    file_write << mt_file.tracker_2_ip << ":" << mt_file.tracker_2_port << endl;
    file_write << mt_file.file_name << endl;
    file_write << mt_file.file_size << endl;
	file_write.write(buffer, 20);
    
    file_write.close();
	file_read.close();



	return mt_file;
}