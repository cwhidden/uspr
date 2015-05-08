CC=g++
CC64=CC
CFLAGS=-O2 -march=native -std=c++0x
OMPFLAGS=-fopenmp
C64FLAGS=$(CFLAGS)
BOOST_GRAPH=-Ilibs/boost/graph/
BOOST_ANY=-L/lib/libboost*
LFLAGS=$(BOOST_GRAPH)
DEBUGFLAGS=-g -O0 -std=c++0x
PROFILEFLAGS=-pg
OBJS=uspr
all: $(OBJS)

uspr: uspr.cpp *.h
	$(CC) $(LFLAGS) $(CFLAGS) -o uspr uspr.cpp

.PHONY: debug
.PHONY: profile
.PHONY: test

debug:
	$(CC) $(LFLAGS) $(DEBUGFLAGS) -o uspr uspr.cpp
profile:
	$(CC) $(LFLAGS) $(DEBUGFLAGS) $(PROFILEFLAGS) -o uspr uspr.cpp
test:
	./uspr < test_trees/trees3.tre
