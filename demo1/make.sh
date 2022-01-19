#!/bin/bash

g++ main.cpp -o main -I/opt/ffmpeg/build/include/ -lavcodec -lavformat -lavutil -L/opt/ffmpeg/build/lib

