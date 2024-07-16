#!/bin/bash

g++ -shared -fPIC -o libps5.so Pad.cpp PS5.cpp

sudo cp libps5.so /lib64
sudo cp libps5.so /usr/lib/pad
sudo cp libps5.so /lib/pad

rm ./libps5.so
