CXX=g++
CXXFLAGS=-g -Wall


z1857628_project5: assign5driver.o
	$(CXX) $(CXXFLAGS) -o z1857628_project5 assign5driver.o

assign5driver.o: assign5driver.cpp MemoryChunk.h PageTable.h PhysicalMemory.h
	$(CXX) $(CXXFLAGS) -c assign5driver.cpp

clean:
	-rm *.o z1857628_project5