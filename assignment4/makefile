CXX=g++
CXXFLAGS=-g -Wall -pthread

all: z1857628_project4_p1 z1857628_project4_p2

z1857628_project4_p1: z1857628_project4_p1.o
	$(CXX) $(CXXFLAGS) -o z1857628_project4_p1 z1857628_project4_p1.o

z1857628_project4_p1.o : z1857628_project4_p1.cpp
	$(CXX) $(CXXFLAGS) -c z1857628_project4_p1.cpp

z1857628_project4_p2: z1857628_project4_p2.o
	$(CXX) $(CXXFLAGS) -o z1857628_project4_p2 z1857628_project4_p2.o

z1857628_project4_p2.o : z1857628_project4_p2.cpp
	$(CXX) $(CXXFLAGS) -c z1857628_project4_p2.cpp

clean:
	-rm *.o z1857628_project4_p1 z1857628_project4_p2