# Linux Makefile
# Socket class for server and client
#

CC  := gcc
CXX := g++
CFLAGS := -g -c
CXXFLAGS := -g -c 
LDFLAGS := -lssl -lcrypto -pthread
DEPS := Socket.h
TARGETS := ForkMirrorServerTest.out

ALL := $(TARGETS)
$(info $$TARGETS = $(TARGETS))

%.o: %.cc $(DEPS)
	$(CXX) $(CXXFLAGS) $<

%.out: %.o Socket.o
	$(info $$^ = $^)
	$(CXX) $^ Socket.o $(LDFLAGS) -o $^

ForkMirrorServerTest.out: ForkMirrorServerTest.o Socket.o
	$(CXX)  ForkMirrorServerTest.o Socket.o $(LDFLAGS) -o $@

ThreadMirrorServer.out: ThreadMirrorServer.o Socket.o
	$(CXX)  ThreadMirrorServer.o Socket.o $(LDFLAGS) -o $@

MirrorClientTest.out: MirrorClientTest.o Socket.o
	$(CXX)  MirrorClientTest.o Socket.o $(LDFLAGS) -o $@

clean:
	rm -f *.o *.out

