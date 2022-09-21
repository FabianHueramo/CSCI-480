/*************************************************************
  CSCI 480                Assignment 4             Fall 2021

  PROGRAM:  z1857628_project4_p1.cpp
  AUTHOR:   Fabian Hueramo
  ZID:      z1857628
  DUE DATE: 10-30-21

  PURPOSE:  Practice with the reader-writer problem using the 
            PThreads library. Practice using semaphores. Does
            not require strict alternating of writers & readers.
*************************************************************/
#include <iostream>
#include <semaphore.h>
#include <pthread.h>
#include <thread>
#include <signal.h>
#include <chrono>
#include <cstdio>
using namespace std;

//Global variables
string sharedStr = "All work and no play makes Jack a dull boy.";
int read_count = 0;     //tracks # of current readers
sem_t rw_sem;           //used by both reads and writers
sem_t cs_sem;           //used for protecting critical sections of readers

/*********************************************************************
  FUNCTION:   void *writer(void *threadID)

  ARGUMENTS:  [void]*threadID: thread ID of thread that called funciton.

  RETURNS:    [void]

  PURPOSE:    The logic to be performed by writer threads. Re-writes 
              the string without the last character.
*********************************************************************/
void *writer(void *threadID)
{
    long tid = (long)threadID;

    //loop until the string is empty
    while(sharedStr.length() != 0)
    {
        //get rw lock
        sem_wait(&rw_sem);

        //exit while loop if string is empty
        if(sharedStr.length() == 0)
        {
            sem_post(&rw_sem);
            continue;
        }

        //display writing (chop off last character of the string)
        printf("writer %ld is writing\n", tid);
        sharedStr = sharedStr.substr(0, sharedStr.size() - 1);

        //release rw lock - sleep 1 second
        sem_post(&rw_sem);
        this_thread::sleep_for(chrono::seconds(1));
    }

    //thread finished - close thread
    printf("writer %ld is exiting...\n", tid);
    pthread_exit(NULL);
}

/*********************************************************************
  FUNCTION:   void *reader(void *threadID)

  ARGUMENTS:  [void]*threadID: thread ID of thread that called funciton.

  RETURNS:    [void]

  PURPOSE:    The logic to be performed by reader threads. Reads the
              shared string and displays its content. Keeps count of
              # of readers.
*********************************************************************/
void *reader(void *threadID)
{
    long tid = (long)threadID;

    //loop until the string is empty
    while(sharedStr.length() != 0)
    {
        //get rw lock
        sem_wait(&rw_sem);

        //exit while loop if string is empty
        if(sharedStr.length() == 0)
            continue;

        //entering critical section - get cs lock
        sem_wait(&cs_sem);

        //increment and display read_count
        read_count++;
        printf("read_count increments to: %d.\n", read_count);

        //display contents read - decrement and display read_count
        printf("reader %ld is reading ... content : %s \n", tid, sharedStr.c_str());
        read_count--;
        printf("read_count decrements to : %d.\n", read_count);

        //exit critical section - relase cs lock
        sem_post(&cs_sem);

        //release rw lock - sleep for 1 second
        sem_post(&rw_sem);
        this_thread::sleep_for(chrono::seconds(1));
    }
    //thread finished - close thread
    printf("reader %ld is exiting...\n", tid);
    pthread_exit(NULL);
}

//main program
int main(int argc, char *argv[])
{
    int numRThreads;    //number of reader threads specified
    int numWThreads;    //number of writer threads specified
    int returnCode;     //code returned when creating a thread

    //get command line arguements - error check
    if(argc > 3)
    {
        cerr << "error: too many arguments\n";
        exit(1);
    }
    if(argc < 3)
    {
        cerr << "error: missing argument(s)\n";
        exit(1);
    }
    numRThreads = atoi(argv[1]);
    numWThreads = atoi(argv[2]);

    //initialization of semaphores (both set to 1) - error check
    if(sem_init(&rw_sem, 0, 1) != 0)
    {
        cerr << "error: initialization failed\n";
        exit(1);
    }
    if(sem_init(&cs_sem, 0, 1) != 0)
    {
        cerr << "error: initialization failed\n";
        exit(1);
    }

    //display program header
    cout << "*** Reader-Writer Problem Simulation ***\n";
    cout << "Number of reader threads: " << numRThreads << endl;
    cout << "Number of writer threads: " << numWThreads << endl;

    //create thread arrays for reading and writing
    pthread_t rThreads[numRThreads];
    pthread_t wThreads[numWThreads];

    //create reader threads - get return code - error check
    for(long i = 0; i < numRThreads; i++)
    {
        returnCode = pthread_create(&rThreads[i], NULL, reader, (void *)i);
        if(returnCode)
        {
            cerr << "error: return code from pthread_create() is " << returnCode << endl;
            exit(1);
        }
    }

    //create writer threads - get return code - error check
    for(long i = 0; i < numWThreads; i++)
    {
        returnCode = pthread_create(&wThreads[i], NULL, writer, (void *)i);
        if(returnCode)
        {
            cerr << "error: return code from pthread_create() is " << returnCode << endl;
            exit(1);
        }
    }

    //wait for reader threads to finish
    for(long i = 0; i < numRThreads; i++)
        pthread_join(rThreads[i], NULL);

    //wait for writer threads to finish
    for(long i = 0; i < numWThreads; i++)
        pthread_join(wThreads[i], NULL);

    //cleanup and exit
    sem_destroy(&rw_sem);
    sem_destroy(&cs_sem);
    cout << "All threads are done.\n";
    cout << "Resources cleaned up.\n";
    return 0;
}