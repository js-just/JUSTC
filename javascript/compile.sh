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
set -e

OUTPUT_DIR="${1:-development}"
SAFE_DIR=$(echo "$OUTPUT_DIR" | sed 's|/|_|g') || "${{ env.DEFAULT_DIR }}"
mkdir -p "javascript/$SAFE_DIR"

echo $OUTPUT_DIR
echo $SAFE_DIR
echo "dirpath=$SAFE_DIR" >> $GITHUB_OUTPUT

wget -q https://github.com/WebAssembly/wabt/releases/download/1.0.34/wabt-1.0.34-ubuntu.tar.gz
tar -xzf wabt-1.0.34-ubuntu.tar.gz
sudo cp wabt-1.0.34/bin/* /usr/local/bin/
rm -rf wabt-1.0.34 wabt-1.0.34-ubuntu.tar.gz

web() {
    set +e
    emcc core/js.cpp core/lexer.cpp core/parser.cpp core/json.cpp core/keywords.cpp core/fetch.cpp \
        -o javascript/$SAFE_DIR/justc.core.js \
        -s EXPORTED_FUNCTIONS='["_lexer","_parser","_parse","_free_string","_malloc","_free"]' \
        -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","UTF8ToString","stringToUTF8"]' \
        -s MODULARIZE=1 \
        -s ALLOW_MEMORY_GROWTH=1 \
        -s INVOKE_RUN=0 \
        -std=c++11 \
        -s DISABLE_EXCEPTION_CATCHING=0 \
        -s EXPORT_NAME='__justc__' \
        -s ASSERTIONS=0 \
        -s ASYNCIFY=1 \
        -s FETCH=1 \
        -s ASYNCIFY_IMPORTS=['fetch','emscripten_fetch','emscripten_fetch_close'] \
        -O3 \
        -flto \
        -s TOTAL_STACK=8MB \
        -s TOTAL_MEMORY=32MB \
        -s ENVIRONMENT='web,worker' \
        -s MODULARIZE=1 \
        -s AGGRESSIVE_VARIABLE_ELIMINATION=1 \
        -s MAXIMUM_MEMORY=256MB
    COMPILE_EXIT_CODE=$?
    set -e

    if [ $COMPILE_EXIT_CODE -ne 0 ]; then
        echo "Failed to compile."
        exit 1
    fi
}

node() {
    set +e
    emcc core/js.cpp core/lexer.cpp core/parser.cpp core/json.cpp core/keywords.cpp core/fetch.cpp \
        -o javascript_output/$SAFE_DIR/justc.node.js \
        -s EXPORTED_FUNCTIONS='["_lexer","_parser","_parse","_free_string","_malloc","_free"]' \
        -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","UTF8ToString","stringToUTF8"]' \
        -s MODULARIZE=1 \
        -s ALLOW_MEMORY_GROWTH=1 \
        -s INVOKE_RUN=0 \
        -std=c++11 \
        -s DISABLE_EXCEPTION_CATCHING=0 \
        -s EXPORT_NAME='__justc__' \
        -s ASSERTIONS=0 \
        -s ASYNCIFY=1 \
        -s FETCH=1 \
        -s ASYNCIFY_IMPORTS=['fetch','emscripten_fetch','emscripten_fetch_close'] \
        -O3 \
        -flto \
        -s TOTAL_STACK=8MB \
        -s TOTAL_MEMORY=32MB \
        -s ENVIRONMENT='node' \
        -s MODULARIZE=1 \
        -s AGGRESSIVE_VARIABLE_ELIMINATION=1 \
        -s MAXIMUM_MEMORY=256MB
    COMPILE_EXIT_CODE=$?
    set -e

    if [ $COMPILE_EXIT_CODE -ne 0 ]; then
        echo "Failed to compile."
        exit 1
    fi
}

web
mkdir javascript_output && mkdir javascript_output/$SAFE_DIR && \
node

mv javascript/$SAFE_DIR/justc.core.wasm javascript_output/$SAFE_DIR/justc.wasm
sed -i 's/justc\.core\.wasm/justc.wasm/g' javascript/$SAFE_DIR/justc.core.js
mv javascript/$SAFE_DIR/justc.core.js javascript_output/$SAFE_DIR/justc.core.js
mv javascript/core.js javascript_output/$SAFE_DIR/justc.js
mv javascript/test.js javascript_output/$SAFE_DIR/test.js

for file in javascript_output/$SAFE_DIR/justc.core.js javascript_output/$SAFE_DIR/justc.js javascript_output/$SAFE_DIR/justc.node.js; do
    printf "/*\n\n%s\n\n*/\n\n" "$(cat LICENSE)" | cat - "$file" > temp.js && mv temp.js "$file"
done
for file in justc justc.node; do
    wasm2wat javascript_output/$SAFE_DIR/$file.wasm > javascript_output/$SAFE_DIR/$file.wat
    {
        head -n1 "javascript_output/$SAFE_DIR/$file.wat"
        echo "  (@custom \"justc\" \"Just an Ultimate Site Tool Configuration language\")"
        echo "  (@custom \"justc.website\" \"https://just.js.org/justc\")"
        echo "  (@custom \"justc.license\" \"MIT License. https://just.js.org/justc/license.txt\")"
        echo "  (@custom \"justc.copyright\" \"Copyright (c) 2025 JustStudio. <https://juststudio.is-a.dev/>\")"
        tail -n +2 "javascript_output/$SAFE_DIR/$file.wat"
    } > javascript_output/$SAFE_DIR/$file.tmp
    wat2wasm javascript_output/$SAFE_DIR/$file.tmp --enable-annotations -o javascript_output/$SAFE_DIR/$file.wasm
    rm javascript_output/$SAFE_DIR/$file.wat javascript_output/$SAFE_DIR/$file.tmp
done

ls -la javascript_output/$SAFE_DIR/
file javascript_output/$SAFE_DIR/justc.core.wasm
hexdump -C javascript_output/$SAFE_DIR/justc.core.wasm | head -20

mv javascript/test.html javascript_output/$SAFE_DIR/test.html
mv javascript/test.justc javascript_output/$SAFE_DIR/test.justc

for FILE in javascript_output/*; do
  echo "::debug::$FILE"
done
