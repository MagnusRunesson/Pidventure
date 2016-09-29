CC=gcc
CXX=clang++
#CFLAGS= -O0 -g -std=c++11 -mmacosx-version-min=10.11 -Wextra -Wall -F/Library/Frameworks -I../include -I-../include
CFLAGS=-O0 -g -std=c++11 -mmacosx-version-min=10.9 -F/Library/Frameworks -I../include -I-../include
LIBS=-framework SDL2 -framework SDL2_image -lc++ -lc
LDFLAGS=-macosx_version_min 10.9 -framework SDL2 -framework SDL2_image


CFILES=png2petb.cpp

OBJS=$(CFILES:.cpp=.o)

all: build install

install: build
	sudo cp png2petb /usr/local/bin
	
build: $(OBJS)
	$(LD) $(LDFLAGS) -o png2petb $(LIBS) $^

%.o: %.cpp
	$(CXX) $(CFLAGS) -c -o $@  $<

clean:
	rm -f png2petb $(OBJS) *.o
