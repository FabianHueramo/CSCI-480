/********************************************************
  CSCI 480            Assignment 1           Fall 2021

  PROGRAM:   assign1.cpp
  AUTHOR:    Fabian Hueramo
  LOGON ID:  Z1857628
  DUE DATE:  09-09-21

  PURPOSE:   Practice reading virtual/proc files
********************************************************/
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <set>
#include <iterator>
#include <sstream>
using namespace std;

//Function declaration
string getTuringOSInfo(string filename);

//main program
int main()
{
    int processors = 0;			//total # of processors
    int sessionLength = 0;		//how long turing has been up in seconds
    int sessionTemp = 0;		//temporary holder for session length
    int sessionDays = 0;		//days session has been up
    int sessionHours = 0;		//hours session has been up
    int sessionMin = 0;			//minutes session has been up
    int sessionSec = 0; 		//seconds session has been up
    int userModeTime = 0;		//time spent in user mode in seconds
    int systemModeTime = 0;		//time spent in system mode in seconds
    int idleModeTime = 0; 		//time spent in idle mode in seconds
    int idleTimeTemp = 0;		//copy of time spent in indle mode
    int idleDays = 0;			//days being idle
    int idleHours = 0;			//hours being idle
    int idleMin = 0;			//minutes being idle
    int idleSec = 0;			//seconds being idle (remaining, not conversion)
    int swapsSizeMB = 0;		//size of swap device in megabytes
    string userModeHZ;			//number of interrupts in user mode
    string systemModeHZ;		//number of interrupts in system mode
    string idleModeHZ;			//number of interrupts in in idle mode
    string buffer;			//used to read in files line by line
    string vendorID;			//processor vendor id
    string modelName;			//processor model name
    string vAddrSize;			//virtual address size
    string pAddrSize;			//physical address size
    string swapSizeKB;			//size of swap device in kilobytes
    static const size_t npos = -1;	//returned by find() when no pattern found
    set<int> uniqueIDs;			//contains physical IDs

    //call getTuringOSInfo() to get info from passed file parameter
    string ostype = getTuringOSInfo("/proc/sys/kernel/ostype");
    string hostname = getTuringOSInfo("/proc/sys/kernel/hostname");
    string osrelease = getTuringOSInfo("/proc/sys/kernel/osrelease");
    string version = getTuringOSInfo("/proc/sys/kernel/version");

    //display info about turing's OS
    cout << "A: Questions about turing's OS:" << endl;
    cout << "1. The type of turing's OS is " << ostype << "." <<  endl;
    cout << "2. The hostname is " << hostname << "." << endl;
    cout << "3. The osrelease is " << osrelease << "." << endl;
    cout << "4. The version is " << version << "." << endl << endl;

    //open file - error checking
    ifstream cpuinfoFile("/proc/cpuinfo");
    if(!cpuinfoFile)
    {
	cerr << "Error: opening `/proc/cpuinfo`" << endl;
        exit(1);
    }

    //while not at end of file...
    while( !cpuinfoFile.eof() )
    {
	//read line into buffer
	getline(cpuinfoFile, buffer);

        //if line contains word processor, increment total processor count
        size_t pfound = buffer.find("processor");
        if(pfound != npos){ processors++; }

	//if line contains word 'physical id', add id to set of uniqueIDs
	size_t idFound = buffer.find("physical id");
	if(idFound != npos) { uniqueIDs.insert(buffer.back()); }

        //store vendor id
	size_t vendorFound = buffer.find("vendor_id");
	if(vendorFound != npos)
	{
	    int pos = buffer.find(":");
	    vendorID = buffer.substr(pos + 1);
	}

	//store model name
	size_t modelFound = buffer.find("model name");
	if(modelFound != npos)
        {
	    int pos = buffer.find(":");
	    modelName = buffer.substr(pos + 1);
	}

	//store physical and virtual address size
	size_t addrFound = buffer.find("address sizes");
	if(addrFound != npos)
	{
	    int pos = buffer.find(":") + 1;
	    pAddrSize = buffer.substr(pos, 3);
	    vAddrSize = buffer.substr(34, 2);
	}
    }
    cpuinfoFile.close();

    //get time turing has been up
    ifstream uptimeFile("/proc/uptime");
    if(!uptimeFile)
    {
	cerr << "Error: opening `/proc/uptime`" << endl;
	exit(1);
    }
    uptimeFile >> sessionLength;
    uptimeFile.close();

    //open /proc/stat file
    ifstream statFile("/proc/stat");
    if(!statFile)
    {
	cerr << "Error: opening `/proc/uptime`" << endl;
	exit(1);
    }

    //while not at end of /proc/stat file...
    while(!statFile.eof())
    {
	//read line from file into buffer - find line about cpu5
	getline(statFile, buffer);
	size_t procFound = buffer.find("cpu5");
	if(procFound != npos)
	{
	    //get number of ticks per mode
	    userModeHZ = buffer.substr(5,6);
	    systemModeHZ = buffer.substr(17,6);
	    idleModeHZ = buffer.substr(24, 9);

	    //convert user mode interrupts to seconds
	    stringstream userTemp(userModeHZ);
	    userTemp >> userModeTime;
	    userModeTime /= 100;

	    //convert system mode interrupts to seconds
	    stringstream systemTemp(systemModeHZ);
	    systemTemp >> systemModeTime;
	    systemModeTime /= 100;

	    //convert idle mode interrupts to seconds
	    stringstream idleTemp(idleModeHZ);
	    idleTemp >> idleModeTime;
	    idleModeTime /= 100;

	    //format idle mode time in days, hours, minutes, seconds
	    idleTimeTemp = idleModeTime;
	    idleDays = idleTimeTemp / 86400;
	    idleHours = ((idleTimeTemp % 86400) / 3600);
	    idleMin = (((idleTimeTemp % 86400) % 3600) / 60);
	    idleSec = (((idleTimeTemp % 86400) % 3600) % 60);
	}
    }

    //format session length in days, hours, minutes, seconds
    sessionTemp = sessionLength;
    sessionDays = sessionTemp / 86400;
    sessionHours = ((sessionTemp % 86400) / 3600);
    sessionMin = (((sessionTemp % 86400) % 3600) / 60);
    sessionSec = (((sessionTemp % 86400) % 3600) % 60);

    //open /proc/swaps file
    ifstream swapsFile("/proc/swaps");
    if(!swapsFile)
    {
	cerr << "error: opening `/procs/swaps` file";
	exit(1);
    }

    //read in first line then overrite with second line, extract size
    getline(swapsFile, buffer);
    getline(swapsFile, buffer);
    swapSizeKB = buffer.substr(50, 8);

    //convert size to megabytes
    stringstream swapTemp(swapSizeKB);
    swapTemp >> swapsSizeMB;
    swapsSizeMB /= 1024;

    //turing processor questions
    cout << "B: Questions about turing's processors: " << endl;
    cout << "1. Turing has " << processors <<  " processors." << endl;
    cout << "2. Turing has " << uniqueIDs.size() << " multi-core chips." << endl;
    cout << "3. Turing has been up for " << sessionLength << " seconds." << endl;
    cout << "4. Turing has been up " << sessionDays << " days, " << sessionHours << " hours, " << sessionMin;

    //decide to display either  'minute' or 'minutes'
    if(sessionMin == 1)
        cout << " minute, ";
    else
        cout << " minutes, ";

    //display remaining seconds
    cout << sessionSec << " seconds" << endl << endl;

    //display information about processor 0
    cout << "C: Questions about processor 0: " << endl;
    cout << "1. The vendor is" << vendorID << endl;
    cout << "2. The model name is" << modelName <<  endl;
    cout << "3. The physical address size is" << pAddrSize << " bits" << endl;
    cout << "4. The virtual address size is " << vAddrSize << " bits" << endl << endl;

    //display information about processor 5
    cout << "D. Questions about processor 5: " << endl;
    cout << "1. Time spent in user mode: " << userModeTime << " seconds" << endl;
    cout << "2. Time spent in system mode: " << systemModeTime << " seconds" << endl;
    cout << "3. Time being idle: " << idleModeTime << " seconds" << endl;
    cout << "4. Time being idle: " << idleDays << " days, " << idleHours << " hours, " << idleMin;

    //decide to display either `minute` or `minutes`
    if(idleMin == 1)
	cout << " minute, ";
    else
	cout << " minutes, ";

    //display remaining seconds
    cout << idleSec << " seconds" << endl << endl;

    //display size of turing's swap device in MB
    cout << "E. Size of turing's swap device: " << swapsSizeMB << " megabytes" << endl;

    return 0;
}

/*****************************************************************
  FUNCTION:   string getTuringOSINFO(string filename)

  ARGUMENTS:  [string]filename: name of file to read from

  RETURNS:    [string]info: the data read from filename

  NOTES:      Assumes file only contains one line of info.
*****************************************************************/
string getTuringOSInfo(string filename)
{
    string info;	//info to return

    //open file for reading - check for errors
    ifstream file(filename);
    if(!file)
    {
	cerr << "Error: opening `" << filename << "`" << endl;
	exit(1);
    }

    getline(file, info);  //store file contents into info
    file.close();	  //close file
    return info;
}

