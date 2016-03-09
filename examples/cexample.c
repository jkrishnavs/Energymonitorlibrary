#include <stdio.h>
#include "energylib.h"

#define SIZE 10000

int main(int argc, char **argv) {
  
  printf("Start Monitoring");
  /* record data every 1 sec .*/
  
  energymonitor__init(__ALL_CORES,1);
  energymonitor__startprofiling();
  int i;
  int arr[SIZE]
    /** dummy work **/
  for(i=0;i<SIZE;i++){
    arr[i] = i;
  }
  for(i=0;i<SIZE;i++){
    arr[i] /= 2;
  }
  
  /* stop monitoring*/
  printf("stop monitoring");
  energymonitor__stopprofiling();

}
