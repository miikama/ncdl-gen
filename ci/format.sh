#!/bin/bash

set -e
current_directory=$(dirname $(realpath $0))
repository_root=$(dirname $current_directory)

for source_file in $(find $repository_root/{src,test} -name *.cpp -or -name *.h); do
    echo "formatting $source_file"
    clang-format -i -style=file:$repository_root/.clang-format  "$source_file"
done