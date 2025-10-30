#!/bin/bash

sudo apt-get update
sudo apt-get install -y libcurl4-openssl-dev cmake build-essential pkg-config

mkdir build
cd build
cmake ..
make
sudo make install

hash -r
if ! command -v justc &> /dev/null; then
    sudo ln -sf /usr/local/bin/justc /usr/bin/justc
    hash -r
fi

FILE=../test/linux/test

echo ""
echo ""
echo "running JUSTC"
echo ""
echo "--version:"
justc --version
echo ""
echo "--help:"
justc --help
echo ""
echo "exeute:"
justc $FILE.justc $FILE.json --results
echo ""
echo "lexer:"
justc $FILE.justc $FILE.lexer.json --results --lexer
echo ""
echo "parser:"
justc $FILE.justc $FILE.parse.json --results --parser

echo ""
echo "reading outputs"
RESULT=$(cat $FILE.json)
RESULT2=$(cat $FILE.lexer.json)
RESULT3=$(cat $FILE.parse.json)
echo "Executed:  $RESULT"
echo "Parsed:    $RESULT3"
echo "Tokenized: $RESULT2"
