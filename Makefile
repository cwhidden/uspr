CC=g++
CC64=CC
CFLAGS=-O2 -march=native -std=c++0x
OMPFLAGS=-fopenmp
C64FLAGS=$(CFLAGS)
BOOST_GRAPH=-Ilibs/boost/graph/include
BOOST_ANY=-L/lib/libboost*
LFLAGS=$(BOOST_GRAPH)
DEBUGFLAGS=-g -O0 -std=c++0x
PROFILEFLAGS=-pg
OBJS=uspr uspr_neighbors

.PHONY: debug
.PHONY: profile
.PHONY: test
.PHONY: test_trees/%.test
.PHONY: clean
.PHONY: all
.PHONY: default

all: $(OBJS)

all-tests := $(addsuffix .test_out, $(basename $(wildcard test_trees/*.test)))

uspr: uspr.cpp *.h
	$(CC) $(LFLAGS) $(CFLAGS) -o uspr uspr.cpp

uspr_neighbors: uspr_neighbors.cpp *.h
	$(CC) $(LFLAGS) $(CFLAGS) -o uspr_neighbors uspr_neighbors.cpp

debug:
	$(CC) $(LFLAGS) $(DEBUGFLAGS) -o uspr uspr.cpp
profile:
	$(CC) $(LFLAGS) $(DEBUGFLAGS) $(PROFILEFLAGS) -o uspr uspr.cpp
test: $(all-tests)
	@echo "Success, all tests passed."

test_trees/%.test_out : test_trees/%.tre test_trees/%.test uspr
	@./uspr <$< 2>&1 > $@ && diff -1 $(addsuffix .test, $(basename $<)) $@ || \
	(echo "Test $@ failed" && exit 1)
	@echo "Test $@ passed"

clean:
	@rm $(OBJS) test_trees/*.test_out 2> /dev/null || echo > /dev/null
