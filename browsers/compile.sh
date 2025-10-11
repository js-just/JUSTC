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
mkdir -p "browsers/$SAFE_DIR"

echo $OUTPUT_DIR
echo $SAFE_DIR
echo "dirpath=$SAFE_DIR" >> $GITHUB_OUTPUT

wget -q https://github.com/WebAssembly/wabt/releases/download/1.0.34/wabt-1.0.34-ubuntu.tar.gz
tar -xzf wabt-1.0.34-ubuntu.tar.gz
sudo cp wabt-1.0.34/bin/* /usr/local/bin/
rm -rf wabt-1.0.34 wabt-1.0.34-ubuntu.tar.gz

set +e
emcc core/browsers.cpp core/lexer.cpp core/parser.cpp core/json_serializer.cpp core/keywords.cpp core/fetch.cpp \
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
    -s ASYNCIFY=1 \
    -s FETCH=1 \
    -s ASYNCIFY_IMPORTS=['emscripten_fetch','emscripten_fetch_close'] \
    -Os
COMPILE_EXIT_CODE=$?
set -e

if [ $COMPILE_EXIT_CODE -ne 0 ]; then
    echo "Failed to compile."
    exit 1
fi

mv browsers/$SAFE_DIR/justc.core.wasm browsers/$SAFE_DIR/justc.wasm
sed -i 's/justc\.core\.wasm/justc.wasm/g' browsers/$SAFE_DIR/justc.core.js
mv browsers/core.js browsers/$SAFE_DIR/justc.js
rm browsers/compile.sh

for file in browsers/$SAFE_DIR/justc.core.js browsers/$SAFE_DIR/justc.js; do
    printf "/*\n\n%s\n\n*/\n\n" "$(cat LICENSE)" | cat - "$file" > temp.js && mv temp.js "$file"
done
wasm2wat browsers/$SAFE_DIR/justc.wasm > browsers/$SAFE_DIR/justc.wat
{
    head -n1 "browsers/$SAFE_DIR/justc.wat"
    echo "  (@custom \"justc\" \"Just an Ultimate Site Tool Configuration language\")"
    echo "  (@custom \"justc.website\" \"https://just.js.org/justc\")"
    echo "  (@custom \"justc.license\" \"MIT License. https://just.js.org/justc/license.txt\")"
    echo "  (@custom \"justc.copyright\" \"Copyright (c) 2025 JustStudio. <https://juststudio.is-a.dev/>\")"
    tail -n +2 "browsers/$SAFE_DIR/justc.wat"
} > browsers/$SAFE_DIR/justc.tmp
wat2wasm browsers/$SAFE_DIR/justc.tmp --enable-annotations -o browsers/$SAFE_DIR/justc.wasm
rm browsers/$SAFE_DIR/justc.wat browsers/$SAFE_DIR/justc.tmp

ls -la browsers/$SAFE_DIR/
file browsers/$SAFE_DIR/justc.core.wasm
hexdump -C browsers/$SAFE_DIR/justc.core.wasm | head -20

mv browsers/test.html browsers/$SAFE_DIR/test.html
