# MIT License
#
# Copyright (c) 2025-2026 JustStudio. <https://juststudio.is-a.dev/>
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

FILE=test/linux/test
if ! command -v justc &> /dev/null; then
    echo -e "::error::Failed to compile." && exit 1
fi

hexdump -C $FILE.justc

echo "::group::version"
justc -v
echo "::endgroup::"

echo "::group::help"
justc -h
echo "::endgroup::"

echo "::group::interpret"
justc interpret -p $FILE.justc
echo "::endgroup::"

echo "::group::compile"
justc compile justb -p $FILE.justc $FILE.justb
hexdump -C $FILE.justb
echo "::endgroup::"

echo "::group::execute"
justc execute -p $FILE.justb
echo "::endgroup::"

echo "::group::serialize - JSON"
justc serialize json -p $FILE.justc $FILE.json
hexdump -C $FILE.json
echo "::endgroup::"

echo "::group::serialize - JUSTO"
justc serialize justo -p $FILE.justc $FILE.justo
hexdump -C $FILE.justo
echo "::endgroup::"

echo "::group::serialize - XML"
justc serialize xml -p $FILE.justc $FILE.xml
hexdump -C $FILE.xml
echo "::endgroup::"

echo "::group::serialize - YAML"
justc serialize yaml -p $FILE.justc $FILE.yaml
hexdump -C $FILE.yaml
echo "::endgroup::"

echo "::group::benchmark"

declare -a outputModes=("json" "justo" "xml" "yaml")
for outputMode in "${outputModes[@]}"; do
    
    timeName="JUSTC -> ${outputMode^^}"
    (
        start=$(date +%s%3N)

        justc serialize $outputMode -p $FILE.justc

        end=$(date +%s%3N)
        duration=$((end - start))
        echo "${timeName}: ${duration}ms"
    ) &
done

wait

echo "::endgroup::"
