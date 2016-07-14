.PHONY:all clean

all:test

SRC=ring_buffer.cpp test_ring_buffer.cpp crc32.cpp

CPPFLAGS=-Wall -g -O3 -Werror 

LIBS=-lpthread

test:$(SRC)
	$(CXX) $^ -o $@ $(CPPFLAGS) $(LIBS)

clean:
	rm -fr test
