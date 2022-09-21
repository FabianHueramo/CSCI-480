/***********************************************
  Driver for Virtual Memory Simulation
  CSCI 480 Fall 2021
  Northern Illinois University
***********************************************/

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
//the header files of the related classes and data structure
//you can modify based on your filenames
#include "MemoryChunk.h"  //contains the struct for a PageTableEntry
#include "PhysicalMemory.h" //contains the class PhysicalMemory
#include "PageTable.h"  //contains the class PageTable

using namespace std;

vector<string> virtualMemoryList; //layout of the virtual memory
vector<string> runList; //page reference string

//load the virtual memory with each item
void loadVirtualMemory()
{
	virtualMemoryList.push_back("main");
	virtualMemoryList.push_back("load");
	virtualMemoryList.push_back("tax");
	virtualMemoryList.push_back("update");
	virtualMemoryList.push_back("print");
	virtualMemoryList.push_back("holding");
	virtualMemoryList.push_back("activity");
	virtualMemoryList.push_back("net");
}

//load each item to be referenced
void loadRunList()
{
	//load the reference string
	runList.push_back("main");
	runList.push_back("load");
	runList.push_back("holding");
	runList.push_back("main");
	runList.push_back("activity");
	runList.push_back("net");
	runList.push_back("main");
	runList.push_back("update");
	runList.push_back("holding");
	runList.push_back("net");
	runList.push_back("activity");
	runList.push_back("main");
	runList.push_back("tax");
	runList.push_back("net");
	runList.push_back("main");
	runList.push_back("print");
	runList.push_back("net");
}

//this function accepts a string item name and returns the item's
//matching virtual page index
int itemToPageID(string itemIn)
{
	//for each item in the virtual memory list
	for(int i = 0; i < virtualMemoryList.size(); i++)
	{
		//get the current virtual memory list item
		string cItem = virtualMemoryList.at(i);

		//compare the requested run list item with the current virtual memory list item
		int sComp = cItem.compare(itemIn);

		//if the two items match
		if (sComp == 0)
		{
			//return the current virtual memory list index
			return i;
		}
	}

	//if we get here, no match was found
	return -1;
}

int main(int argc, char *argv[])
{
	int physicalMemSize = 4; //you can change as a command line argument
	int algorithm = 2; //1 for FIFO, 0 for LRU, 2 for LIFO
    //int algorithm = 0;  //1 for FIFO, 0 or other number for LRU if implemented.
	  
	if (argc == 1) 
	{
		cout << "Default to four frames. You can change via command line argument: ";
		cout << "Usage: ./a.out <number of frames>" << endl;
	}
	else
	    physicalMemSize = atoi(argv[1]);

	cout << "Welcome!" << endl;

	loadVirtualMemory();
	loadRunList();

	PhysicalMemory physicalMem(physicalMemSize, algorithm); 
	PageTable pTable(&physicalMem, virtualMemoryList.size());

	if (algorithm == 1) //you can add LRU-related printout if implemented
		cout << "FIFO with "<< physicalMemSize <<" physical frames" << endl;
	else if (algorithm == 0) //you can add LRU-related printout if implemented
		cout << "LRU with "<< physicalMemSize <<" physical frames" << endl;
	else if (algorithm == 2)
		cout << "LIFO with " << physicalMemSize << " physical frames" << endl;

    
	//for each item in the run list
	for(int i = 0; i < runList.size(); i++)
	{
		//fetch the virtual page index for the string item
		int virtualIdx = itemToPageID(runList.at(i));

		//if the virtual page index was found
		if(virtualIdx != -1)
			//reference the item at the specified virtual index
			pTable.reference(virtualIdx, runList.at(i));
		//otherwise the item to virtual index failed
		else
		{
			//print an error message and break
			cout << "Failed to convert reference to virtual memory: " << runList.at(i) << endl;
			break;
		}
	}

	cout << "Now print snapshots:" << endl << endl;

	//print physical memory and page table
	physicalMem.printMemory();
	pTable.printTable();

    return 0;
}