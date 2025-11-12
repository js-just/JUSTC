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

SOURCE_FILES="core/js.cpp core/lexer.cpp core/parser.cpp core/from.json.cpp core/to.json.cpp core/keywords.cpp core/fetch.cpp core/to.xml.cpp core/to.yaml.cpp core/utility.cpp core/import.cpp core/run.lua.cpp"

COMMON_FLAGS="-s EXPORTED_FUNCTIONS=[\"_lexer\",\"_parser\",\"_parse\",\"_free_string\",\"_malloc\",\"_free\",\"_version\"] \
-s EXPORTED_RUNTIME_METHODS=[\"ccall\",\"cwrap\",\"UTF8ToString\",\"stringToUTF8\"] \
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
-s TOTAL_MEMORY=64MB \
-s AGGRESSIVE_VARIABLE_ELIMINATION=1 \
-s MAXIMUM_MEMORY=512MB \
--bind \
-I./third-party"

WEB_FLAGS="-s ENVIRONMENT=web,worker"
WEB_OUTPUT="javascript/$SAFE_DIR/justc.core.js"

NODE_FLAGS="-s ENVIRONMENT=node"
NODE_OUTPUT="javascript_output/$SAFE_DIR/justc.node.js"

JSOUT_DIR="javascript_output/$SAFE_DIR"

web() {
    set +e
    emcc $SOURCE_FILES \
        -o $WEB_OUTPUT \
        $COMMON_FLAGS \
        $WEB_FLAGS
    COMPILE_EXIT_CODE=$?
    set -e

    if [ $COMPILE_EXIT_CODE -ne 0 ]; then
        echo "Failed to compile."
        exit 1
    fi
}

node() {
    mkdir -p $JSOUT_DIR
    set +e
    emcc $SOURCE_FILES \
        -o $NODE_OUTPUT \
        $COMMON_FLAGS \
        $NODE_FLAGS
    COMPILE_EXIT_CODE=$?
    set -e

    if [ $COMPILE_EXIT_CODE -ne 0 ]; then
        echo "Failed to compile."
        exit 1
    fi
}

web && \
node

mv javascript/$SAFE_DIR/justc.core.wasm $JSOUT_DIR/justc.wasm
sed -i 's/justc\.core\.wasm/justc.wasm/g' javascript/$SAFE_DIR/justc.core.js
mv javascript/$SAFE_DIR/justc.core.js $JSOUT_DIR/justc.core.js
mv javascript/core.js $JSOUT_DIR/justc.js
mv javascript/test.js $JSOUT_DIR/test.js

JUSTC_VERSION=$(justc -v 2>/dev/null || echo "undefined")
JUSTC_NAME="Just an Ultimate Site Tool Configuration language"

if [[ "$JUSTC_VERSION" == "undefined" ]]; then
    echo -e "::error::Invalid JUSTC version." && exit 1
fi
for file in $JSOUT_DIR/justc.core.js $JSOUT_DIR/justc.js $JSOUT_DIR/justc.node.js; do
    printf "/*\n\n%s\n\n*/\n\n/*\n\n$JUSTC_NAME v$JUSTC_VERSION ($SAFE_DIR)\n\n*/\n\n" "$(cat LICENSE)" | cat - "$file" > temp.js && mv temp.js "$file"
done
for file in justc justc.node; do
    wasm2wat $JSOUT_DIR/$file.wasm > $JSOUT_DIR/$file.wat
    {
        head -n1 "$JSOUT_DIR/$file.wat"
        echo "  (@custom \"justc\" \"$JUSTC_NAME\")"
        echo "  (@custom \"justc.website\" \"https://just.js.org/justc\")"
        echo "  (@custom \"justc.license\" \"MIT License. https://just.js.org/justc/license.txt\")"
        echo "  (@custom \"justc.copyright\" \"Copyright (c) 2025 JustStudio. <https://juststudio.is-a.dev/>\")"
        echo "  (@custom \"justc.version\" \"$JUSTC_VERSION ($SAFE_DIR)\")"
        tail -n +2 "$JSOUT_DIR/$file.wat"
    } > $JSOUT_DIR/$file.tmp
    wat2wasm $JSOUT_DIR/$file.tmp --enable-annotations -o $JSOUT_DIR/$file.wasm
    rm $JSOUT_DIR/$file.tmp
done

mkdir -p $JSOUT_DIR/JUSTC/core
mkdir -p $JSOUT_DIR/JUSTC/javascript
srcfile=$JSOUT_DIR/JUSTC/index.json
echo "[" > $srcfile
SOURCE_FILES+=" core/main.cpp core/lexer.h core/parser.h core/from.json.hpp core/to.json.h core/keywords.h core/fetch.h core/version.h core/json.hpp core/to.xml.h core/to.yaml.h core/utility.h core/import.hpp core/parser.emscripten.h core/run.js.cpp core/run.js.hpp core/run.lua.hpp"
for file in $SOURCE_FILES; do
    if [ -f "$file" ]; then
        echo "\"$file\"," >> $srcfile
        mkdir -p $JSOUT_DIR/JUSTC/$(dirname $file)
        cp $file $JSOUT_DIR/JUSTC/$file
    fi
done
head -c-2 $srcfile > $srcfile.tmp && mv $srcfile.tmp $srcfile
echo "]" >> $srcfile
cp javascript/core.txt $JSOUT_DIR/JUSTC/javascript/core.js
cp javascript/index.d.txt $JSOUT_DIR/JUSTC/javascript/core.d.ts
OUTPUT_URL="https://just.js.org/justc/$SAFE_DIR"
echo "{\"version\":3,\"file\":\"$OUTPUT_URL/justc.js\",\"sources\":[\"$OUTPUT_URL/JUSTC/javascript/core.js\",\"$OUTPUT_URL/JUSTC/javascript/core.d.ts\"],\"mappings\":\"\"}" > $JSOUT_DIR/justc.js.map
printf "%s\n//# sourceMappingURL=$OUTPUT_URL/justc.js.map" "$(cat $JSOUT_DIR/justc.js)" > temp.js && mv temp.js "$JSOUT_DIR/justc.js"

mv javascript/test.html $JSOUT_DIR/test.html
mv javascript/test.justc $JSOUT_DIR/test.justc

cp $JSOUT_DIR/justc.js $JSOUT_DIR/index.js
mv javascript/index.d.ts $JSOUT_DIR/index.d.ts
mv javascript/npm.json $JSOUT_DIR/package.json

for FILE in $JSOUT_DIR/*; do
    echo "::debug::$FILE"
done

ls -la $JSOUT_DIR/
file $JSOUT_DIR/justc.core.wasm
hexdump -C $JSOUT_DIR/justc.core.wasm | head -20
