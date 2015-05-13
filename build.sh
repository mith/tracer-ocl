#! /bin/sh

mkdir release
cd release
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release ..
ninja
