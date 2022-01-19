#!/bin/bash

g++ main.cpp -o main -I/usr/local/include/ffmpeg/ -lavcodec -lavformat -lavutil -L/opt/ffmpeg/build/lib

