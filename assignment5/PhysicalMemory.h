/***************************************************************
    CSCI 480                Assignment 5             Fall 2021

    FILE:       PhysicalMemory.h
    AUTHOR:     Fabian Hueramo
    ZID:        z1857628
    DUE DATE:   11-19-21

    PURPOSE:    Simulates the physical memory using the 
                PhysicalMemory class. Contains methods for page
                replacement and memory access.
***************************************************************/
#ifndef PHYSICALMEMORY_H
#define PHYSICALMEMORY_H
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

//class that simulates physical memory
class PhysicalMemory
{
    public:
        PhysicalMemory(int memorySize, int algorithmIn);    //initialize all data and data structure in class. The second argument is for testing different
                                                            //page replacement algorithms if you will implement the bonus credits for LRU.

        void access(int frameID);           //access the frame. If LRU update the time stamp and time list.
        void printMemory() const;           //print out the physical memory layout.
        int swapLn(string item);            //returns the frame ID of the item just swapped in. If FIFO update
                                            //time stamp and time list.

    private:
        vector <string> memoryList;     //The physical memory!
        vector <int> timeList;          //data structure for the replacement algorithm
        int currentTimeIndex;           //current clock for timestamp
        int algorithmFlag;              //indicate if using FIFO or LRU
        int getNextAvailableFrame();    //get a frame, either available or via replacement
        bool isFull();                  //check if the memory is full
};

/******************************************************************************
    FUNCTION:   PhysicalMemory(int memorySize, int algorithmIn)

    ARGUMENTS:  [int] memorySize - size to set memoryList
                [int] algorithmIn - specifies which algorithm to use for page
                                    replacement.

    PURPOSE:    Initializes all the data and data structure in the class. 
*******************************************************************************/
PhysicalMemory::PhysicalMemory(int memorySize, int algorithmIn)
{
    //set memoryList (physical memory) and timeList size to memorySize 
    memoryList.resize(memorySize);
    timeList.resize(memorySize);  

    //set algorithm flag (1 = FIFO 0 = LRU)
    algorithmFlag = algorithmIn;    

    //set clock to 0
    currentTimeIndex = 0;           
}

/*****************************************************************************
    FUNCTION:   void access(int frameID)

    ARGUMENTS:  [int] frameID - the index of the frame trying to access

    RETURNS:    [void]

    PURPOSE:    Accesses the memoryList item at index frameID and prints
                the frame accessed and what it contained. Increments the
                currentTimeIndex for timeList if using LRU algorithm for
                page replacement.
*****************************************************************************/
void PhysicalMemory::access(int frameID)
{
    //access string at frameID from memoryList 
    string strAccessed = memoryList.at(frameID);

    //print frameID accessed and what it contains
    cout << "Physical: Accessed frameID: " << frameID << " contains: " << strAccessed << endl << endl;

    //using LRU algorithm
    if(algorithmFlag != 1)
    {
        //increment time index - set for accesed frame
        currentTimeIndex++;
        timeList.at(frameID) = currentTimeIndex;
    }
}

/*********************************************************************
    FUNCTION:   void printMemory() const

    RETURNS:    [void]

    PURPOSE:    Prints layout of physical memory.
*********************************************************************/
void PhysicalMemory::printMemory() const
{
    cout << "Physical Memory Layout:" << endl;
    
    //loop through memory list - print contents
    for(int i = 0; i < memoryList.size(); i++)
    {
        cout << "Frame: " << i << " contains: " << memoryList.at(i) << endl;
    }
    cout << endl;
}

/*********************************************************************
    FUNCTION:   int swapLn(string item)

    ARGUMENTS:  [string] item - that item to be swapped into physical
                memory.

    RETURNS:    [int] frameID of item just swapped into physical memory.

    PURPOSE:    Swaps the item into physical memory at next availble
                frame.
*********************************************************************/
int PhysicalMemory::swapLn(string item)
{
    //get next available frame
    int frame = getNextAvailableFrame();

    //swap line in physical memory at frameID frame with item
    memoryList.at(frame) = item;
    
    //return frameID of item just swapped in
    return frame;
}

/*********************************************************************
    FUNCTION:   int getNextAvailableFrame()

    ARGUMENTS:  none

    RETURNS:    [int] framdID of next available frame

    PURPOSE:    Gets the next avaialble frame in memory. If memory is
                full, performs either FIFO or LRU algorithm to decide
                vitcim frame.
*********************************************************************/
int PhysicalMemory::getNextAvailableFrame()
{
    //available frame to be returned
    int frameID;

    //loop through physical memory and search for empty frames
    for(int i = 0; i < memoryList.size(); i++)
    {
        //if frame is empty - return frameID
        if((memoryList.at(i)).empty())
        {
            //increment currentTimeIndex and add to time list
            currentTimeIndex++;
            timeList.at(i) = currentTimeIndex;

            return i;
        }
    }

    //no empty frame found - perform page replacement algorithm

    //perform FIFO algorithm
    if(algorithmFlag == 1)
    {
        //get first in index
        int victimIndex = min_element(timeList.begin(), timeList.end()) - timeList.begin();

        //increment time index - insert time index at corresponding memoryList index
        currentTimeIndex++;
        timeList.at(victimIndex) = currentTimeIndex;

        //return index of victim
        return victimIndex;
    }

    //perform LRU algorithm (exact same logic as FIFO, added for concept clarity) - modified after assignment
    else if(algorithmFlag == 0)
    {
        //get last recently used index
        int victimIndex = min_element(timeList.begin(), timeList.end()) - timeList.begin();

        //increment time index - insert time index at corresponding memoryList index
        currentTimeIndex++;
        timeList.at(victimIndex) = currentTimeIndex;

        //return index of victim
        return victimIndex;
    }

    //perform LIFO algorithm (extra, not part of assignmnent)
    else if(algorithmFlag == 2)
    {
        //get last frame entered
        int victimIndex = max_element(timeList.begin(), timeList.end()) - timeList.begin();

        //incremet time index - insert time index at corresponding memoryList index
        currentTimeIndex++;
        timeList.at(victimIndex) = currentTimeIndex;

        //return index of victim
        return victimIndex;
    }

    //shouldn't get here
    return -1;
}

/*********************************************************************
    FUNCTION:   bool isFull()

    RETURNS:    [bool] true if memory is full, false if not full

    PURPOSE:    Used to check if memory list is full.
*********************************************************************/
bool PhysicalMemory::isFull()
{
    //loop through memory list
    for(int i = 0; i < memoryList.size(); i++)
    {
        //if memory list index i is empty
        if((memoryList.at(i)).empty())
        {
            //memory list is not full
            return false;
        }
    }

    //memory list is full
    return true;
}

#endif