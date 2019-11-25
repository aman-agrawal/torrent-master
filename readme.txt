makefile.server

makefile.client


on three different terminals : 
-----------------------------

./server 127.0.0.1:8080 127.0.0.1:8080 seeders.txt log_file_server.txt

./client 127.0.0.1:8085 127.0.0.1:8080 127.0.0.1:8080 log_file_client_8085.txt

./client 127.0.0.1:8086 127.0.0.1:8080 127.0.0.1:8080 log_file_client_8086.txt


share /home/aman/Desktop/Sem1/OS/Assignment/torrent-master/aman.txt aman.mtorrent

get aman.mtorrent /home/aman/Desktop
