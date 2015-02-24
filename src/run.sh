#!/bin/bash

#build the executable file
clear;
if [ -z $1 ]
then
	make;
else
	make obj=$1;
fi

if [ $? -eq 0 ]
then
    clear;
    #run the executable file and then
    #delete the executable file
    if [ -z $1 ]
    then
    	./cplus;
    	rm cplus;
    else
    	./$1;
    	rm $1;
    fi
fi

make clean;
