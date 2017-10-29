#!/bin/bash

GREEN_TXT='\e[0;32m'
WHITE_TXT='\e[1;37m'
RED_TXT='\e[31m'

printf "${GREEN_TXT}\n***Installing zbar for python Package***\n${WHITE_TXT}"


sudo apt-get update 
sudo apt-get install python-zbar
#END
