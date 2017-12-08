#!/bin/bash

set -e

script_dir=$(dirname `readlink -f $0`)

proj_name=$(grep -Po '(?<="name": ")[^"]*' config.json)
make_tool=$(grep -Po '(?<="make_tool": ")[^"]*' config.json)

#pre build
if [[ -f $script_dir/prebuild.sh ]]; then
    echo "===== start prebuild ====="
    bash $script_dir/prebuild.sh
    echo "===== end prebuild ====="
fi

rm -rf $script_dir/build $script_dir/build_ut
mkdir -p build
cd build
echo "===== start build ${proj_name} ====="
conan install ../src --build --update
cp ../ci.example.external.pri ../external.pri
${make_tool} ../src
make
cd ..

if [ -d "test" ]; then
    cp ./ci.example.test.external.pri ./test.external.pri
    mkdir -p build_test
    cd build_test
    echo "===== start build unit test ====="
    conan install ../test --build --update
    ${make_tool} ../test
    make
    cd ..
fi

#post build
if [[ -f $script_dir/postbuild.sh ]]; then
    echo "===== start postbuild ====="
    bash $script_dir/postbuild.sh
    echo "===== end postbuild ====="
fi
