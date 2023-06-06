OPENSSL_DIR = /opt/homebrew/opt/openssl@3

CXX = g++
CFLAGS = -pthread -std=c++11 -Wall -I$(OPENSSL_DIR)/include -lssl 
LDFLAGS =

SOURCES = main.cpp 
OBJECTS = main.o

OPENSSL_SUPPORT = -L$(OPENSSL_DIR)/lib

crawler: $(OBJECTS)
	$(CXX) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o crawler $(OPENSSL_SUPPORT)

main.o: main.cpp 
	$(CXX) -c $(CFLAGS) -o main.o main.cpp

clean:
	rm -f *.o
	rm -f crawler