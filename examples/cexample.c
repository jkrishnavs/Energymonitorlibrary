#include <stdio.h>
#include "energylib.h"

int main(int argc, char **argv) {
  
  printf("Start Monitoring");
  /* record data every 1 sec .*/
  energymonitor__init(cores,1);
  energymonitor__startprofiling();
  
  // do work..
  /* stop monitoring*/
  printf("stop monitoring");
  energymonitor__stopprofiling();

}
