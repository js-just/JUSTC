#!/bin/bash

sudo apt-get update
sudo apt-get install -y libcurl4-openssl-dev cmake build-essential pkg-config

mkdir build
cd build
cmake ..
make
sudo make install

FILE=../test/linux/test

justc $FILE.justc $FILE.json

RESULT=$(cat $FILE.json)
echo $RESULT
