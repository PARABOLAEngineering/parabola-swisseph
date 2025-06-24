#!/bin/bash
make clean
rm -r build
mkdir build
cmake .
make