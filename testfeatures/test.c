#include <stdio.h>
#include "energylib.h"
#include <time.h>
#include <stdlib.h>
#include <omp.h>
#define SIZE 10000
int arr[SIZE];

void foo(){
    /** dummy work **/
  int i;
  srand(time(0));

#pragma omp parallel for private (i)
  for(i=0;i<SIZE;i++){
    arr[i] = rand();
    usleep(1000);
  }
  int temp;
#pragma omp parallel for private(i,temp)
  for(i=0;i<SIZE/2;i++){
    temp = arr[i];
    arr[i] = arr[SIZE-i-1];
    arr[SIZE-i-1] = temp;
    usleep(1000);
  }
  
  /* stop monitoring*/
}


int main(int argc, char **argv) {
  
  printf("Start Monitoring\n");
  /* record data every 1 sec .*/
  

  energymonitor__saveascsv("csvfile_poweronly.csv");
  energymonitor__trackpoweronly();
  energymonitor__init(__ALL_CORES,1);
  energymonitor__startprofiling();
  foo();
  energymonitor__stopprofiling();


  energymonitor__saveascsv("csvfile_powervolt.csv");
  energymonitor__trackvoltage();
  energymonitor__init(__ALL_CORES,1);
  energymonitor__startprofiling();
  foo();
  energymonitor__stopprofiling();



  energymonitor__saveascsv("csvfile_all.csv");
  energymonitor__trackeverything();
  energymonitor__init(__ALL_CORES,1);
  energymonitor__startprofiling();
  foo();
  energymonitor__stopprofiling();


  energymonitor__saveascsv("csvfile_powervolt_pause.csv");
  energymonitor__trackvoltage();
  energymonitor__init(__ALL_CORES,1);  
  energymonitor__startprofiling();  
  foo();
  energymonitor__pauseprofiling();
  foo();
  energymonitor__unpauseprofiling();
  foo();
  energymonitor__stopprofiling();
  

  energymonitor__saveascsv("csvfile_all__14track.csv");
  energymonitor__trackeverything();
  energymonitor__init(__ALL_CORES,1);
  energymonitor__settrackingcores(1|4);
  energymonitor__startprofiling();  
  foo();
  energymonitor__stopprofiling();
 

  /** Text file save ***/

  energymonitor__saveastextfile("textfilefile_poweronly.txt");
  energymonitor__trackpoweronly();
  energymonitor__init(__ALL_CORES,1);
  energymonitor__startprofiling();
  foo();
  energymonitor__stopprofiling();


  energymonitor__saveastextfile("textfilefile_powervolt.txt");
  energymonitor__trackvoltage();
  energymonitor__init(__ALL_CORES,1);
  energymonitor__startprofiling();
  foo();
  energymonitor__stopprofiling();



  energymonitor__saveastextfile("textfilefile_all.txt");
  energymonitor__trackeverything();
  energymonitor__init(__ALL_CORES,1);
  energymonitor__startprofiling();
  foo();
  energymonitor__stopprofiling();


  energymonitor__saveastextfile("textfilefile_powervolt_pause.txt");
  energymonitor__trackpoweronly();
  energymonitor__init(__ALL_CORES,1);  
  energymonitor__startprofiling();  
  foo();
  energymonitor__pauseprofiling();
  foo();
  energymonitor__unpauseprofiling();
  foo();
  energymonitor__stopprofiling();
  

  energymonitor__saveastextfile("textfilefile_vol.txt");
  energymonitor__trackvoltage();   
  energymonitor__init(__ALL_CORES,1);
  energymonitor__settrackingcores(1|4);
  energymonitor__startprofiling();  
  foo();
  energymonitor__stopprofiling();

  
  printf("Stop monitoring, end of test\n,%d",arr[1]);
 

  return 0;
}
