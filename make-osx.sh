#!/bin/bash

mkdir -p Release-OSX
cd Release-OSX

cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release
make
