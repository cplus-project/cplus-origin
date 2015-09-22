#!/bin/bash
# type [./run] to run the main cplus compiler program
# type [./run test/????] to run a test file

# build the executable file

typeset _CPLUS_TEST_FILE_;
typeset _CPLUS_TEST_EXEC_;

_CPLUS_TEST_FILE_=$1;

clear;
if [ -z ${_CPLUS_TEST_FILE_} ]
then
    make;
else
    make mainfile=${_CPLUS_TEST_FILE_};
fi

if [ $? -eq 0 ]
then
    clear;
    # run the executable file and then
    # delete the executable file
    if [ -z ${_CPLUS_TEST_FILE_} ]
    then
    	./cplus;
    	rm cplus;
    else
    	_CPLUS_TEST_EXEC_=${_CPLUS_TEST_FILE_:0:${#_CPLUS_TEST_FILE_}-2};
    	./${_CPLUS_TEST_EXEC_};
    	rm ${_CPLUS_TEST_EXEC_};
    fi
fi

make clean;

unset _CPLUS_TEST_FILE_;
unset _CPLUS_TEST_EXEC_;

