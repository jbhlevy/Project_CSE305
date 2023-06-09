LIBCURL_DIR = /usr/local/opt/curl

LDLIBS += -lcurl

CXX = g++
CFLAGS = -pthread -std=c++11 -Wall -O3 -I$(OPENSSL_DIR)/include -I$(LIBCURL_DIR)include
LDFLAGS = 

SOURCES = main.cpp 
OBJECTS = main.o


crawler: $(OBJECTS)
	$(CXX) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o crawler $(LDLIBS)

main.o: main.cpp 
	$(CXX) -c $(CFLAGS) -o main.o main.cpp

clean:
	rm -f *.o
	rm -f crawler