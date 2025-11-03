# MIT License
# 
# Copyright (c) 2025 JustStudio. <https://juststudio.is-a.dev/>
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

#!/bin/bash

bash linux/compile.sh
FILE=test/linux/testttt

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
echo "execute:"
justc $FILE.justc $FILE.json -r
echo ""
echo "lexer:"
justc $FILE.justc $FILE.lexer.json -r -l
echo ""
echo "parse:"
justc $FILE.justc $FILE.parse.json -r -p
echo ""
echo "parser:"
justc $FILE.lexer.json $FILE.parser.json -r -P
echo ""
echo "parserExecute:"
justc $FILE.lexer.json $FILE.parsEr.json -r -E

echo ""
echo "reading outputs"
RESULT=$(cat $FILE.json)
RESULT2=$(cat $FILE.lexer.json)
RESULT3=$(cat $FILE.parse.json)
RESULT4=$(cat $FILE.parser.json)
RESULT5=$(cat $FILE.parsEr.json)
echo "Executed:           $RESULT"
echo "Parsed (string):    $RESULT3"
echo "Tokenized:          $RESULT2"
echo "Parsed (tokens):    $RESULT4"
echo "Parsed (tokens,-E): $RESULT5"

echo ""
echo "executing JUSTC to XML"
justc $FILE.justc $FILE.xml -r -x
echo "reading output"
RESULTX=$(cat $FILE.xml)
echo "Executed (XML):     $RESULTX"

echo ""
echo "executing JUSTC to YAML"
justc $FILE.justc $FILE.yml -r -y
echo "reading output"
RESULTX=$(cat $FILE.yml)
echo "Executed (YAML):    $RESULTX"
