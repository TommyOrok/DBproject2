# Makefile


CC = gcc
CXX = g++

INCLUDES =

CFLAGS = -g -Wall $(INCLUDES)
CXXFLAGS = -g -Wall $(INCLUDES)

# Linking
LDFLAGS = -g

# List the libraries you need to link with in LDLIBS
LDLIBS =


build: build.o
build.o: build.c


#build: build.o random.o
#build.o: build.c
#random.o: random.c


#p2random: p2random.o
#p2random.o: p2random.c


#p2randomv2: p2randomv2.o
#p2randomv2.o: p2randomv2.c



.PHONY: clean
clean:
	rm -f *.o a.out core main p2random p2randomv2 random build alt

.PHONY: all
all: clean main
