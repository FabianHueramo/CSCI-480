/***************************************************************
    CSCI 480                Assignment 5             Fall 2021

    FILE:       MemoryChunk.h
    AUTHOR:     Fabian Hueramo
    ZID:        z1857628
    DUE DATE:   11-19-21

    PURPOSE:    Simulates a page table entry.
***************************************************************/
#ifndef MEMORYCHUNK_H
#define MEMORYCHUNK_H
using namespace std;

//simulates a page table entry
struct PageTableEntry
{
    bool valid;                 //is item entry references in memory
    int physicalMemoryIndex;    //item entry references
};

#endif