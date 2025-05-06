#!/usr/bin/bash

g++ -I/usr/include/libhackrf -L/usr/lib -o record record.cpp -lhackrf
echo "done"
