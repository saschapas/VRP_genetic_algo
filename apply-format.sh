#!/usr/bin/env sh

CLANG_FORMAT=clang-format

SOURCES=`find src -type f \( -name '*.cpp' -o -name '*.hpp'  \)`

for f in $SOURCES; do
    echo "Formatting:" $f
    $CLANG_FORMAT -style=file -i $f
done
