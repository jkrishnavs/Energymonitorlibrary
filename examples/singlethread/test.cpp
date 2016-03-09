#include<cstdio>
#include "energylib.h"




int main(){
  int i,j,k;
  EnergyMonitor::energymonitor__initialize();
  EnergyMonitor::energymonitor_settrackingcores(__CORES_CPU0|__CORES_CPU1|__CORES_CPU4|__CORES_CPU5);
  EnergyMonitor::energymonitor__setsleeptime(1);
  EnergyMonitor::energymonitor__startprofiling();
  for(i=0;i<10000;i++){
    k=0;
    for(j=0;j<100000;j++){
      k++;
    }
  }
  printf("%d",k);
  EnergyMonitor::energymonitor__stopprofiling();
  return 0;
}

