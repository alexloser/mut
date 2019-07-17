#!/bin/bash
build="g++ -std=c++11 -I. example.cc -o example"
echo $build
$build
echo "run example of mut"
./example
