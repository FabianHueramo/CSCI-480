/***************************************************************
    CSCI 480                Assignment 5             Fall 2021

    FILE:       PageTable.h
    AUTHOR:     Fabian Hueramo
    ZID:        z1857628
    DUE DATE:   11-19-21

    PURPOSE:    Simulates the page table using the PageTable 
                class. Contains methods for handling page faults
                and updating/modigying page table.
***************************************************************/
#ifndef PAGETABLE_H
#define PAGETABLE_H
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

//class that simulates page table
class PageTable
{
    public:
        PageTable(PhysicalMemory* pmIn, size_t tableSize);  //constructor
        void reference(int pageID, string item);            //reference a logical page, if not in memory, call pageFault()
        int getFaults() const;                              //return # of faults
        void printTable() const;                            //print the layout of page table
        void reset();                                       //reset validity flag to false, and numberoffaults to 0

    private:
        vector<PageTableEntry> entryList;           //the page table
        int numFaults;
        PhysicalMemory* mainPhysicalMemory;         //used to call PhysicalMemory methods
        void pageFault(int pageID, string item);    //called when there is a page fault
        void updateReplacedEntry(int replacedIndex, int pageID);    //after page replacement, update the page table correspondingly
                                                                    //by making the original mapping for the frame invalid
};

/******************************************************************************
    FUNCTION:   PageTable(PhysicalMemory* pmIn, size_t tableSize)

    ARGUMENTS:  [PhysicalMemory*] pmIn - pointer to physical memory list.
                [size_t] tableSize - desired size of page table

    RETURNS:    [void]

    PURPOSE:    Constructor for PhysicalMemory class. Intializes all entries
                to invalid.
*******************************************************************************/
PageTable::PageTable(PhysicalMemory* pmIn, size_t tableSize)
{
    entryList.resize(tableSize);    //resize entryList (page table) to tableSize
    mainPhysicalMemory = pmIn;      //set pointer to physical memory
    reset();                        //initialize all entries to invalid - initialize physical memory address
    
}

/******************************************************************************
    FUNCTION:   reference(int pageID, string item)

    ARGUMENTS:  [int] pageID - page ID of pageEntry to reference
                [string] item - the item of pageEntry to reference

    RETURNS:    [void]

    PURPOSE:    References a logical page, if not in memory, calls pageFault(). 
*******************************************************************************/
void PageTable::reference(int pageID, string item)
{
    //if corresponding entry is valid
    if((entryList.at(pageID)).valid == true)
    {
        //access the entry in Physical Memory
        mainPhysicalMemory->access((entryList.at(pageID)).physicalMemoryIndex);
    }
    //otherwise entry is not valid - call pageFault()
    else 
    { 
        pageFault(pageID, item); 
    }
}

/******************************************************************************
    FUNCTION:   getFaults() cosnt

    RETURNS:    [int] number of page faults

    PURPOSE:    Returns number of page faults
*******************************************************************************/
int PageTable::getFaults() const 
{ 
    return numFaults; 
}

/******************************************************************************
    FUNCTION:   printTable() const

    RETURNS:    [void]

    PURPOSE:    Prints the layout of page table.
*******************************************************************************/
void PageTable::printTable() const
{
    cout << "Page Table Snapshot:\n";
    
    //loop through entryList and print its layout
    for(int i = 0; i < entryList.size(); i++)
    {
        cout << "Page Index: " << i << " : Physical Frame Index: " << (entryList.at(i)).physicalMemoryIndex;
        cout << " : In Use: " << boolalpha << (entryList.at(i)).valid << endl;
    }

    //print number of page fualts
    cout << "PageTable: Current number of page faults: " << getFaults() << endl;
}

/******************************************************************************
    FUNCTION:   reset()

    RETURNS:    [void]

    PURPOSE:    Resets the validity flag to false, and number of faults to 0. 
*******************************************************************************/
void PageTable::reset()
{
    //initialize all entries to invalid - set numFaults to 0
    for(int i = 0; i < entryList.size(); i++)
    {
        entryList[i].valid = false;
        numFaults = 0;
    }
}

/******************************************************************************
    FUNCTION:   void pageFault(int pageID, string item)

    ARGUMENTS:  [int] pageID - page ID of pageEntry
                [string] item - the item of pageEntry

    RETURNS:    [void]

    PURPOSE:    Increments numFaults. Calls swapLn() of the PhysicalMemory class
                to swap in item into physical memory.
*******************************************************************************/
void PageTable::pageFault(int pageID, string item)
{
    cout << "PageTable: page fault occurred" << endl;
    numFaults++;

    //call swapLn() to get frameID of item just swapped in
    int frameID = mainPhysicalMemory->swapLn(item);

    //set frameID for new page entry - set valid bit to true
    (entryList.at(pageID)).physicalMemoryIndex = frameID;
    (entryList.at(pageID)).valid = true;

    //update replaced entry
    for(int i = 0; i < entryList.size(); i++)
    {
        int frame = (entryList.at(i)).physicalMemoryIndex;
        if( (i != pageID) && (frame == frameID) )
        {
            //invalidate page entry that points to recently replaced frame
            (entryList.at(i)).valid = false;
            (entryList.at(i)).physicalMemoryIndex = -1;
        }
    }

    //print swapped and access info
    cout << "Physical: Swap in: " << item << endl;
    mainPhysicalMemory->access(frameID);
}

#endif