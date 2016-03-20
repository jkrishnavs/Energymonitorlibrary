#ifdef __cplusplus
#include<string>
#include<ctime>
#else
#include<string.h>
#include<time.h>
#endif


#define NUMCORES        8
/**
   core ids these macros can be used to create the core masks.
 **/
#define __CORES_CPU0 1
#define __CORES_CPU1 2
#define __CORES_CPU2 4
#define __CORES_CPU3 8
#define __CORES_CPU4 16
#define __CORES_CPU5 32
#define __CORES_CPU6 64
#define __CORES_CPU7 128

#define __ALL_CORES 255

#define __LITTLE_CORES 15

#define __BIG_CORES 240
#ifdef __cplusplus
extern "C"{
#endif
 
  /**
     CTRL-C handler function.
     to close the system files.
   **/
  void ctrl_c_handler(int s);
  /**
     default initialization.
   **/
  void energymonitor__initialize();
  /**
     cores : The cpu mask to denote the cores we are tracking
     sleeptime (in seconds): moderates the frequency of profiling.
   **/
  void energymonitor__init(int cores,float sleeptime);
  /**
     To change the rate of profiling.
   **/
  void energymonitor__setsleeptime(float time);
  /**
     start profiling. For an executable use single start and stop.
     If you want profile multiple disjoint areas of the program use pause and unpause.
   **/
  void energymonitor__startprofiling();
  /**
     stop profiling.
     If we want to 
   **/
  void energymonitor__stopprofiling();
  /**
     This helps us to pause the profiling in the middle of the execution. Once paused,
     to restart the profiling use unpauseprofiling.    
   **/
  void energymonitor__pauseprofiling();
  void energymonotor__upauseprofiling();

  /**
     set 1 to track only the power and voltage of the cores and memory
     Will not record the temerature and Freuency of cores.
   **/
  void energymonitor__trackpoweronly();

  /**
     This function helps us to toglle from  tracking only nergy and voltage 
     to start tracking everything.
   **/
  void energymonitor__trackvoltage();
  void energymonitor__trackeverything();
  int energymonitor__runfunction();
  /**
     Change the tracked cores on the fly.
   **/
  void energymonitor__settrackingcores(int cores);
  void* energymonitor__startprofilingthread(void* threadid);

#ifdef __cplusplus
}
#endif
