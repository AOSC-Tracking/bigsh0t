#!/bin/bash

mkdir -p Release-Linux
cd Release-Linux

cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release
make
