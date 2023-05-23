CXX = g++
CFLAGS = -pthread -std=c++11 -Wall

SOURCES = main.cpp 
OBJECTS = main.o 

crawler: $(OBJECTS)
	$(CXX) $(CFLAGS) -o crawler $(OBJECTS) 

main.o: main.cpp 
	$(CXX) -c $(CFLAGS) -o main.o main.cpp

clean:
	rm -f *.o
	rm -f crawler
