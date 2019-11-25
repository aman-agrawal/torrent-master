#!/bin/sh
cd ~/Desktop/Projects_Github/torrent-master
git init
git add .
git commit -m "first"
git config --global user.email "9412470@gmail.com"
git config --global user.name "aman-agrawal"
git remote add origin https://github.com/aman-agrawal/torrent-master.git
git push -u origin master
git config credential.helper store
git push https://github.com/aman-agrawal/torrent-master.git
