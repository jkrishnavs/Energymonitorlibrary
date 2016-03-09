#include <iostream>
#include "energylib.h"
#include <cstdlib>
#include <unistd.h>
#define SIZE 10000

using namespace std;

int main(int argc, char **argv) {
  
  cout<<"Start Monitoring\n";
  /* record data every 1 sec .*/
  
  int i,j;
  int arr[SIZE];
  srand(time(0));
  
  energymonitor__init(__ALL_CORES,1);
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
  cout<<"Stop monitoring"<<arr[1]<<endl;
  energymonitor__stopprofiling();
  return 0;
}
