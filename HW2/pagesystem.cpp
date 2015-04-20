//Dzu Pham
//pagesystem.cpp
//Purpose: To create a paging system

#include <iostream>
#include <cmath>
#include <fstream>
#include <cstring>
#include <ctime>
#include <new>
#include <list>
#include <cstdlib>
#include "pageh.h"
using namespace std;

const int mainMemory=512; //gobal variable

int main ()
{
	int pageSize, amount=9, pFaults=1, inc=0, mmpSize; //amount=# of programs starting at 0
	char programList[20], programTrace[20];
	string algorithm, programName;
	bool type=1; //prepaging(1) or demand-paging(0)
	ifstream pList, pTrace;

	cout << "Start" << endl;
	cin >> programName;
        cout << "Program List: ";
	cin >> programList; 
	cout << "Program Trace: ";
	cin >> programTrace;
	cout << "Page Size: "; 
	cin >> pageSize;
	cout << "Algorithm: ";
	cin >> algorithm;
	cout << "Demand(1) or Pre(0): ";
	cin >> type;
	char string1[] = ".txt";
	char string2[20];
	char string3[20];
	strcpy(string2,programList);
	strcat(string2,string1);
	strcpy(string3,programTrace);
	strcat(string3,string1);

	try
	{
	  pList.open(string2);
	  if (!pList.good())
	    throw string ("Cannot find Program List file name");
	  pTrace.open(string3);
	  if (!pTrace.good())
	    throw string ("Cannot find Program Trace file name");
	  if (algorithm != "fifo" && algorithm != "lru" && algorithm != "clock")
	    throw string ("Not a valid Algorithm. Either 'fifo', 'lru', or 'clock'");
	  if (type != 1 && type != 0)
	    throw string ("Paging type needs to be either a (1)Demand Paging or (0)Pre-Paging");

	}
	catch (string err)
	{
	  cout << '\n' << "!--"<< err << "--!" << endl;
	  cout << "Ending Program" << endl;
	  exit(EXIT_FAILURE);
	}
	
	mmpSize = mainMemory/(pageSize); //the amount of pages each process can have
	long double *MMPT = new long double[mmpSize+1]; //Main Memory Page Table

	//ifstream pList;
	//pList.open("programlist.txt");
	
	Alist *programL = new Alist[amount];
	for (int i=0; i<= amount; i++)
	{
	  pList >> programL[i].m_pNum >> programL[i].m_pSize; //Entering in the program list
	  programL[i].m_pageSize = pageSize;
          programL[i].m_type = type;
	  programL[i].m_pageNeed = programL[i].m_pSize/pageSize;
	  
	  programL[i].pageTable = new long double[programL[i].m_pageNeed];  //making a page table with the size of (program size)/(page Size)
	  for (int k=0; k<=(programL[i].m_pageNeed); k++)
	  {
	    programL[i].pageTable[k] = inc; //this adds the unique number for each program's page table
	    inc++;
	  }
	}	
	
	//pTrace.open("programtrace.txt");
	
	double *programT = new double[3];
	
	int temp;
	if(algorithm=="fifo")
	  temp=1;
	else if (algorithm=="lru")
	  temp=2;
	else if (algorithm=="clock")
	  temp=3;	

        switch(temp)
	{
	  case 1:
	    pFaults = FIFO(programL, programT, pTrace, MMPT, mmpSize);
	    break;
	  case 2:
	    pFaults = LRU(programL, programT, pTrace, MMPT, mmpSize);
	    break;
	  case 3:
	    pFaults = CBP(programL, programT, pTrace, MMPT, mmpSize);
	    break;
	  default:
	    break;
	}
	
	print(programL, algorithm, pFaults);
	
	pList.close();
	pTrace.close();
	for (int i=0; i<=amount; i++)
	{
	  delete [] programL[i].pageTable;
	}
	delete [] programT;
	delete [] programL;
	delete [] MMPT;
  return 0;
}















