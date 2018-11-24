#!/bin/bash 

while true 
do
        rm -f ./nc_tmp
        mkfifo ./nc_tmp
        /bin/sh -c 'read message; [ x"$message" = x"hello" ] && echo "hello"' <./nc_tmp 2>&1 | nc -l -p 2018 > ./nc_tmp
done

