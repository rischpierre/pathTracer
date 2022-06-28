
CC=g++
USD_INSTALL_ROOT=/opt/USD
CFLAGS=-Wall -I$(USD_INSTALL_ROOT)/include -I/usr/include/python3.8 -L$(USD_INSTALL_ROOT)/lib

build:
	$(CC) $(CFLAGS) main.cpp -o pathTracer

all: build
