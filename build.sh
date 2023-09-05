#!/bin/bash

rm -f uart.exe
g++ uart.cpp -lpthread -o uart.exe

