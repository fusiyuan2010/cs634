#!/bin/sh

OPT=-O2 
DEBUG=-g

CFLAGS=-std=c++11 -Wall -Wno-format -fPIC $(OPT) $(DEBUG) -DNG #-DAP #-DNG #-DREUTERS
CC=g++
LIBS =

OBJS=Parser.o ArticleHandler.o TermGraph.o WordTransformer.o porter_stemmer.o main.o PLSA.o

all: topic_analyzer

topic_analyzer: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(LIB_PATH)
	

%.o: %.cpp
	$(CC) $(INCLUDE_PATH) -c $(CFLAGS) -o $@ $^

clean:
	rm -f *.o topic_analyzer

rebuild: clean all



