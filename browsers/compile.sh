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

SAFE_DIR=$(echo "$OUTPUT_DIR" | sed 's|/|_|g') || "${{ env.DEFAULT_DIR }}"
mkdir -p "browsers/$SAFE_DIR"

echo $OUTPUT_DIR
echo $SAFE_DIR

emcc core/browsers.cpp core/lexer.cpp core/parser.cpp core/json_serializer.cpp core/keywords.cpp \
    -o browsers/$SAFE_DIR/justc.core.js \
    -s EXPORTED_FUNCTIONS='["_lexer","_parser","_parse","_free_string","_malloc","_free"]' \
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","UTF8ToString","stringToUTF8"]' \
    -s MODULARIZE=1 \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s INVOKE_RUN=0 \
    -std=c++11 \
    -s DISABLE_EXCEPTION_CATCHING=0 \
    -s EXPORT_NAME='__justc__' \
    -s ASSERTIONS=0 \
    -Os

mv browsers/$SAFE_DIR/justc.core.wasm browsers/$SAFE_DIR/justc.wasm
sed -i 's/justc\.core\.wasm/justc.wasm/g' browsers/$SAFE_DIR/justc.core.js
mv browsers/$SAFE_DIR/core.js browsers/$SAFE_DIR/justc.js
rm browsers/compile.sh

printf "/*\n\n%s\n\n*/\n\n" "$(cat LICENSE)" | cat - browsers/$SAFE_DIR/justc.core.js > temp.js && mv temp.js browsers/$SAFE_DIR/justc.core.js
