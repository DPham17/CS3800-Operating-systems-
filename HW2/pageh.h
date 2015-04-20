//Dzu Pham
//pageh.h
//Purpose: header file for pagesystem

#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <iostream>
#include <cmath>
#include <fstream>
#include <cstring>
#include <cstdlib>
using namespace std;


struct Alist
{
  int m_pNum; //program number
  int m_pSize; //program size
  int m_pageNeed; //this is the size of each program's array
  int m_pageSize; //page size
  int m_faults; //use for inital count of faults
  bool m_type; //demand or pre paging
  long double *pageTable; //pageTable
};

void *read (ifstream& pTrace, double* programT, Alist *programL) //reads in the program trace and returns the values
{
  int temp, temp2;
  pTrace >> programT[0] >> programT[1];
  if (pTrace.eof())
  {
    programT[0] = -1;
    programT[1] = -1;
  }
  else
  {
    temp = programT[1]/programL[1].m_pageSize; //finds the memory location
    temp2 = programT[0]; //program number
    programT[1] = programL[temp2].pageTable[temp]; //goes to that table and loads it
    if (temp >= programL[temp2].m_pageNeed)
      programL[2] = programL[1];
    else
      programT[2] = programL[temp2].pageTable[temp+1]; //this brings in the next page for pre paging
  }
  return 0;
}

void *initalLoad(ifstream& pTrace, double* programT, long double* MMPT, int mmpSize, Alist* programL)
{
  bool found=false;
  int temp=0;
  MMPT[mmpSize+1] = -1;
  MMPT[0]=0;

  do
  {
    read(pTrace, programT, programL);
    found = false;
    for (int k=0; k<=mmpSize; k++)
    {
      if (MMPT[k]==programT[1])
        found=true;
    }
    if(found==false)
    {
      MMPT[temp]=programT[1];
      programL[0].m_faults++;
      temp++;
    }
  }while(temp <= mmpSize); 
  return 0;
}

bool checkFound(int mmpSize, double* programT, long double* MMPT)
{
  for (int i=0; i<=mmpSize; i++)
  {
    if (MMPT[i]==programT[1])
      return true;
  }
  return false;
}

/*Clock Based Policy*/
/*Pre:*/
/*Post:*/
int CBP (Alist *programL, double* programT, ifstream& pTrace, long double* MMPT, int mmpSize)
{
  int faults=0, currentPage=0, loc;
  bool found;
  bool *usedRecently = new bool [mmpSize];
  initalLoad(pTrace, programT, MMPT, mmpSize, programL);
  faults = programL[0].m_faults;
   
  while (programT[0] != -1)
  {
    read(pTrace, programT, programL);
    found = checkFound(mmpSize, programT, MMPT);
    if(found==false)
    {
      faults++;

      while(usedRecently[currentPage])
      {
        usedRecently[currentPage]=false;
        currentPage++;
        currentPage %= mmpSize;
      }

      MMPT[currentPage] = programT[1];
      usedRecently[currentPage] = true;

      currentPage++;
      currentPage %= mmpSize;

      if (programL[0].m_type==true) //pre-paging
      {
        MMPT[currentPage] = programT[2];
        usedRecently[currentPage] = true;
        currentPage++;
        currentPage %= mmpSize;
      }
    }
    else
    {
      loc=-1;
      do
      {
        loc++;
      }while(MMPT[loc] != programT[1]);
      usedRecently[loc] = true;
    }
    
  }

  delete [] usedRecently;
  return faults;
}

/*First In First Out*/
/*Pre:*/
/*Post:*/
int FIFO (Alist *programL, double* programT, ifstream& pTrace, long double* MMPT, int mmpSize)
{
  int faults=0, first=0; //first = pointer location for FIFO
  bool found=false;
  initalLoad(pTrace, programT, MMPT, mmpSize, programL);
  faults = programL[0].m_faults; //temp to hold the inital page faults  
 
  
  while (programT[0] != -1)
  {
    read(pTrace, programT, programL);
    found = checkFound(mmpSize, programT, MMPT);
    if (found==false && (programT[1] != -1)) //demand paging 
    {
      MMPT[first]=programT[1];
      faults++;
      first++;
      first %= mmpSize;
      if (programL[0].m_type==true) //pre-paging 
      {
        programT[1] = programT[2];
        found = checkFound(mmpSize, programT, MMPT);
        if (found==false)
        {
          MMPT[first]=programT[1];
          first++;
          first %= mmpSize;
        }
      }
    }
  }

  return faults;
}

/*Least Recently Used*/
/*Pre:*/
/*Post:*/
int LRU (Alist *programL, double* programT, ifstream& pTrace, long double* MMPT, int mmpSize)
{
  int faults=0, count=0, location;
  bool found=false;
  initalLoad(pTrace, programT, MMPT, mmpSize, programL);
  faults = programL[0].m_faults;
  double *lastUsed = new double[mmpSize];
  
  for (int k=0; k<=mmpSize; k++) //sets the first inital count
  {
    lastUsed[k]=1;
  }
  while (programT[0] != -1)
  {
    read(pTrace, programT, programL);
    found = checkFound(mmpSize, programT, MMPT);
    if (found==false && (programT[1] != -1))
    {
      faults++;
      count=0; //clears the count 
      for (int i=0; i<=mmpSize; i++) //this looks for the first largest count
      {
        if (lastUsed[i] > count)
        {
          location = i;
          count = lastUsed[i];
        }
      }
      MMPT[location] = programT[1]; //replace the LRU
      lastUsed[location] = 0; //resets the location with a new count

      if (programL[0].m_type==true)
      {
        programT[1] = programT[2];
        found = checkFound(mmpSize, programT, MMPT);
        if (found==false)
        {
          count=0;
          for (int i=0; i<=mmpSize; i++)
          {
            if (lastUsed[i] > count)
            {
              location = i;
              count = lastUsed[i];
            }
          }
          MMPT[location] = programT[1];
          lastUsed[location] = 0;
        }
      }

    }
    else if (found==true && (programT[1] != -1))
    {
      location=0; //clears the location
      while(MMPT[location] != programT[1])
      {
        location++;
      }
      lastUsed[location] = 0; //resets the frame that was recently used
    }
    for (int o=0; o<=mmpSize; o++) //adds a count for each spot
    {
      lastUsed[o] = lastUsed[o] + 1;
    } 
  }
  
  delete [] lastUsed;
  return faults;
}

void print(Alist *programL, string algorithm, int pFaults)
{
  cout << "------------------------------------------------" << endl;
  cout << "Page Size: " << programL[0].m_pageSize << endl;
  cout << "Replacement Algorithm: " << algorithm << endl;
  if (programL[0].m_type==false)
    cout << "Paging Policy: Demand Paging" << endl;
  else 
    cout << "Paging Policy: Pre-Paging" << endl;
  cout << "Total Page Faults: " << pFaults << endl;
  cout << "------------------------------------------------" << endl;
  return;
}

#endif
