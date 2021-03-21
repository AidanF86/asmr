#!/bin/sh
gcc main.c `sdl2-config --libs` -lSDL2 -lSDL2_image -lpthread -o asmr -lm
