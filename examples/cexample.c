#include <stdio.h>
#include "energylib.h"
#include <time.h>
#include <stdlib.h>
#define SIZE 10000


int main(int argc, char **argv) {
  
  printf("Start Monitoring\n");
  /* record data every 1 sec .*/
  
  int i,j;
  int arr[SIZE];
  srand(time(0));
  
  energymonitor__init(__ALL_CORES,1);
  //energymonitor__trackpoweronly();
  //  energymonitor__trackvoltage();
  energymonitor__startprofiling();
    /** dummy work **/
  for(i=0;i<SIZE;i++){
    arr[i] = rand();
    usleep(1000);
  }
  int temp;
  for(i=0;i<SIZE/2;i++){
    temp = arr[i];
    arr[i] = arr[SIZE-i-1];
    arr[SIZE-i-1] = temp;
    usleep(1000);
  }
  
  /* stop monitoring*/
  printf("Stop monitoring\n,%d",arr[1]);
  energymonitor__stopprofiling();
  return 0;
}
