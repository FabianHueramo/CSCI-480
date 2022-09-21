/*************************************************************
  CSCI 480                Assignment 2             Fall 2021

  PROGRAM:  assign2.cpp
  AUTHOR:   Fabian Hueramo
  ZID:      z1857628
  DUE DATE: 09-27-21

  PURPOSE:  Practice using fork and other system calls to
	        implement a microshell in C/C++, and practice
	        FCFS CPU scheduling algorithm.
*************************************************************/
#include <iostream>
#include <stdio.h>
#include <cstring>
#include <array>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fstream>
#include <fcntl.h>
using namespace std;

//Function prototype
void runFCFS(int cpuBursts);

//main program
int main()
{
    char buffer[1024];			    //stores command from user input
    char* nextToken;                //points to next token when tokenizing buffer
    int status;                     //status code returned by execvp
    pid_t pid;                      //process id
    string cmdPrompt = "myshell>";  //command prompt

    cout << cmdPrompt;      //print first cmd prompt

    //get user input and store into buffer - while buffer is not null..
    while(fgets(buffer, 1024, stdin) != NULL)
    {
        vector<char*> charVec;      //stores tokens parsed from buffer
        string bufString = "";      //string copy of buffer

        //cut newline character when user hits enter
        buffer[strlen(buffer) - 1] = 0;

	    //if buffer contains "quit" or "q" - exit program
	    if(strcmp(buffer, "quit") == 0 || strcmp(buffer, "q") == 0)
	        exit(0);

        //get size of buffer
        int bufSize = sizeof(buffer) / sizeof(char);

        //convert buffer to string
        for(int i = 0; i < bufSize; i++)
        {
            bufString += buffer[i];
        }

        //if buffer contains special command "fcfs"...
        size_t fcfsFound = bufString.find("fcfs");
        if(fcfsFound != string::npos)
        {
            //get arguments by tokenizing buffer into char* vector
            nextToken = strtok(buffer, " ");
            while(nextToken != NULL)
            {
                charVec.push_back(nextToken);
                nextToken = strtok(NULL, " ");
            }

            charVec.push_back(NULL);        //add NULL to end of charVec
            char* argv[charVec.size()];     //declare array of char pointers size of charVec
            int argc = charVec.size();      //get size of charVec

            //copy charVec contents into array of char pointers
            for(int i = 0; i < argc; i++)
                argv[i] = charVec[i];

            //create new process with fork() - if returns less than 0 display error msg
            if((pid = fork()) < 0)
                cerr << "fork error";

            //child process
            else if(pid == 0)
            {
                //if command contains output redirection ">"
                size_t ioFound = bufString.find(">");
                if(ioFound != string::npos)
                {
                    //open file specified as output redirection target, create if does not exist
                    int fd1 = open(argv[argc - 2], O_WRONLY| O_CREAT, 0644);
                    if(fd1 == -1)
                    {
                        cerr << "error opening/creating file" << endl;;
                        exit(1);
                    }

                    //redirect stdout to file descriptor fd1
                    dup2(fd1, STDOUT_FILENO);
                    close(fd1);
                    
                    //number of cpu bursts passed as argument
                    if(argc == 5)
                    {
                        int argVal = atoi(argv[1]);
                        int cpuBursts = argVal;
                        runFCFS(cpuBursts);
                        exit(0);
                    }

                    //number of cpu bursts not specified
                    else if(argc == 4)
                    {
                        runFCFS(5);
                        exit(0);
                    }

                    //too many arguments passed
                    else
                    {
                        cerr << "error running fcfs\n";
                        exit(1);
                    }

                }

                //no output redirection
                else
                {
                    //there is an argument passed
                    if(argc == 3)
                    {
                        //get arguement - convert to integer - pass as parameter to cpuBursts()
                        int argVal = atoi(argv[1]);
                        int cpuBursts = argVal;
                        runFCFS(cpuBursts);
                        exit(1);
                    }

                    //no arguements passed - pass default parameter of 5
                    else if(argc == 2)
                    {
                        runFCFS(5);
                        exit(0);
                    }

                    //too many arguements passed - max is 1
                    else
                    {
                        cerr << "error running fcfs\n";
                        exit(1);
                    }
                }
                
                exit(0);
            }

            //parent process
            if((pid = waitpid(pid, &status, 0)) < 0)
            {
                cerr << "waitpid error";
            }

	        cout << cmdPrompt;      //print prompt on new line
        }

        //quit, q, or fcfs commands not detected
        else
        {
            //split buffer by spaces, add to vector, advance token
            nextToken = strtok(buffer, " ");
            while(nextToken != NULL)
            {
                charVec.push_back(nextToken);
                nextToken = strtok(NULL, " ");
            }

            
            charVec.push_back(NULL);        //add NULL to end of charVec         
            char* argv[charVec.size()];     //declare array of char pointers size of charVec
            int argc = charVec.size();      //get size of charVec

            //copy charVec contents into array of char pointers
            for(int i = 0; i < argc; i++)
                argv[i] = charVec[i];

            //create new process with fork() - if returns less than 0 display error msg
            if((pid = fork()) < 0)
            {
                cerr << "fork error";
                exit(1);
            }

            //child process
            else if(pid == 0)
            {
                //if command contains output redirection ">"
                size_t ioFound = bufString.find(">");
                if(ioFound != string::npos)
                {
                    //open file specified as output redirection target, create if does not exist
                    int fd1 = open(argv[argc - 2], O_WRONLY| O_CREAT, 0644);
                    if(fd1 == -1)
                    {
                        cerr << "error opening/creating file" << endl;;
                        exit(1);
                    }

                    //redirect stdout to file descriptor fd1
                    dup2(fd1, STDOUT_FILENO);
                    close(fd1);

                    char* argvClean[argc - 1];      //create new array

                    //remove ">" and filename
                    charVec.erase(charVec.end() - 2);
                    charVec.erase(charVec.end() - 2);

                    //copy clean command line into new clean argv
                    for(int i = 0; i < argc; i++)
                        argvClean[i] = charVec[i];

                    //execute command - ouput redirected to file
                    status = execvp(argvClean[0], argvClean);
                    
                    //error check for unexecutable commands
                    if(status == -1)
                    {
                        cerr << "couldn't execute: " << buffer << endl;
                        exit(1);
                    }

                    exit(1);
                }

                //no output redirection specified - execute command
                else
                {
                    status = execvp(argv[0], argv);
                    //error check for unexecutable commands
                    if(status == -1)
                    {
                        cerr << "couldn't execute: " << buffer << endl;
                        exit(1);
                    }
                    exit(1);
                }
                
                exit(0);
            }

            //parent process
            if((pid = waitpid(pid, &status, 0)) < 0)
            {
                cerr << "waitpid error";
            }

            cout << cmdPrompt;      //print prompt on new line
        }
    }

    return 0;
}

/*********************************************************************
  FUNCTION:   void runFCFS(int cpuBursts)

  ARGUMENTS:  [int]cpuBursts: number of cpu bursts to generate

  RETURNS:    [void]

  PURPOSE:    Simulates FCFS CPU scheduling simulaition/calculation
              and displays burst times, total wait time, and average
              wait time.
*********************************************************************/
void runFCFS(int cpuBursts)
{
    int *burstTimes = new int [cpuBursts];    //contains wait time of each burst
    int *totalWaitTime = new int [cpuBursts]; //total wait time in ready queue for each burst
    int totalWait = 0;                        //total wait time in ms
    int avgWaitTime = 0;                      //avg wait time in ready queue
    srand(10);                                //seed for random num generator

    //generate random numbers for cpu bursts
    for(int i = 0; i < cpuBursts; i++)
    {
        burstTimes[i] = rand() % 100 + 1;
    }

    //set first burst wait time to 0 - calculate total waiting time for remaining bursts
    totalWaitTime[0] = 0;
    for(int i = 1; i < cpuBursts; i++)
    {
        for(int j = 0; j < i; j++)
        {
            totalWaitTime[i] += burstTimes[j];
        }
    }

    //calculate total waiting time of all bursts
    for(int i = 0; i < cpuBursts; i++)
    {
        totalWait += totalWaitTime[i];
    }

    //calculate average wait time
    avgWaitTime = totalWait / cpuBursts;

    cout << "FCFS CPU scheduling simulation with " << cpuBursts << " processes.\n";
    
    //display burst times
    for(int i = 0; i < cpuBursts; i++){
        cout << "CPU burst: " <<  burstTimes[i] << "ms\n";
    }

    //display burst total waiting time and average waiting time
    cout << "Total waiting time in the ready queue: " << totalWait << " ms\n";
    cout << "Average waiting time in the ready queue: " << avgWaitTime << " ms\n";
}