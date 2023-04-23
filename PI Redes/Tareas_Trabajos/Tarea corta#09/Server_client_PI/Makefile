# Linux Makefile
# Socket class for server and client
#

CC  := gcc
CXX := g++
CFLAGS := -g -c
CXXFLAGS := -g -c 
LDFLAGS := -lssl -lcrypto -pthread
DEPS := Socket.h Parser.h IOManager.h
TARGETS := Client.out

ALL := $(TARGETS)
$(info $$TARGETS = $(TARGETS))

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) $<

%.out: %.o Socket.o
	$(info $$^ = $^)
	$(CXX) $^ Socket.o $(LDFLAGS) -o $^

%.out: %.o Parser.o
	$(info $$^ = $^)
	$(CXX) $^ Parser.o $(LDFLAGS) -o $^

%.out: %.o IOManager.o
	$(info $$^ = $^)
	$(CXX) $^ IOManager.o $(LDFLAGS) -o $^

Client.out: Client.o Socket.o IOManager.o Parser.o
	$(CXX)  Client.o Socket.o IOManager.o Parser.o $(LDFLAGS) -o $@

Server.out: Server.o Socket.o IOManager.o Parser.o
	$(CXX)  Server.o Socket.o IOManager.o Parser.o $(LDFLAGS) -o $@


clean:
	rm -f *.o *.out

