#ifdef __cplusplus
#include<string>
#include<ctime>
#else
#include<string.h>
#include<time.h>
#endif


#define NUMCORES        8
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

  void ctrl_c_handler(int s);
  void energymonitor__initialize();
  void energymonitor__init(int cores,float sleeptime);
  void energymonitor__setsleeptime(float time);
  void energymonitor__startprofiling();
  void energymonitor__stopprofiling();
  void energymonitor__pauseprofiling();
  void energymonotor__upauseprofiling();
  int energymonitor__runfunction();
  void energymonitor__settrackingcores(int cores);
  void* energymonitor__startprofilingthread(void* threadid);

#ifdef __cplusplus
}
#endif
