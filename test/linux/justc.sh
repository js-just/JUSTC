#!/bin/bash
bash linux/compile.sh
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
