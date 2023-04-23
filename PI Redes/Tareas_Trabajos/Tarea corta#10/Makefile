# Linux Makefile
# Socket class for server and client
#

CC  := gcc
CXX := g++
CFLAGS := -g -c
CXXFLAGS := -g -c 
LDFLAGS := -lssl -lcrypto -pthread
DEPS := Socket.h
TARGETS := SSLClient.out

ALL := $(TARGETS)
$(info $$TARGETS = $(TARGETS))

%.o: %.cc $(DEPS)
	$(CXX) $(CXXFLAGS) $<

%.out: %.o Socket.o
	$(info $$^ = $^)
	$(CXX) $^ Socket.o $(LDFLAGS) -o $^

SSLClient.out: SSLClient.o Socket.o
	$(CXX)  SSLClient.o Socket.o $(LDFLAGS) -o $@

SSLServerThread.out: SSLServerThread.o Socket.o
	$(CXX)  SSLServerThread.o Socket.o $(LDFLAGS) -o $@

SSLServerProcess.out: SSLServerProcess.o Socket.o
	$(CXX)  SSLServerProcess.o Socket.o $(LDFLAGS) -o $@

clean:
	rm -f *.o *.out

