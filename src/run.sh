clear;
make;

if [ $? -eq 0 ]
then
    make clean;
    clear;
    ./cplus;
fi

rm cplus;

