#include "energylib.h"
#include "getnode.h"
#include <fstream>
#include <iostream> // cout, endl
#include <cstdlib>	// atoi
#include <csignal>
#include <unistd.h>	// sleep
#include <iomanip>	// cout format
#include <string.h>


extern "C" {
  /*** COMMON VARIABLES *************/ 

#define POS(i) (1<<i)
  float __energymonitor__sleeptime = 5.0;
  int __energymonitor__keepprofiling = STOP_PROFILE;
  GetNode *__energymonitor__getNode;
  time_t __energymonitor__start_time;
  pthread_t thread_id;
  std::ofstream myfile;
  int __energymonitor__trackflag;
  int __energymonitor__bigTracking = 4;
  std::string timeStr = "Time |";
  std::string voltStr = "A7_V | A15_V | MEM_V | GPU_V |";
  std::string powerStr= "A7_W | A15W | MEM_W | GPU_W | Watt_Sum |";
  std::string freqStr = "CPU0_Freq(MHz) | CPUbig_Freq | CPU0_Temp(C) | CPU1_Temp | CPU2_Temp | CPU3_Temp | GPU_Temp |";
  std::string timeCsv = "Time,";
  std::string voltCsv = " A7_V, A15_V, MEM_V, GPU_V,";
  std::string powerCsv= " A7_W, A15W, MEM_W, GPU_W, Watt_Sum,";
  std::string freqCsv = " CPU0_Freq(MHz), CPUbig_Freq, CPU0_Temp(C), CPU1_Temp, CPU2_Temp, CPU3_Temp, GPU_Temp,";
  //  int __energymonitor__trackonlyPower = 0;
  //int __energymonitor__trackVoltage = 1;
  //int __energymonitor__trackTempandFreq = 1;
  int __energymonitor__trackdetails = TRACK_VOLTAGE + TRACK_TEMPERATURE_AND_FREQ 
		   + TRACK_SAVEAS_CSV; 

  char* __energymonitor__filename;

  /*** COMMON VARIABLES *************/ 


  void energymonitor__setfilename(char* profFileName) {
    if(__energymonitor__trackdetails && TRACK_SAVEAS_CSV) {
      __energymonitor__filename = profFileName;
    } else {
      __energymonitor__filename = profFileName;
    }
  }

  void energymonitor__saveascsv(char* profFileName) {
    __energymonitor__trackdetails |= TRACK_SAVEAS_CSV;
    __energymonitor__filename = profFileName;
  }

  void energymonitor__saveastextfile(char* profFileName) {
    __energymonitor__trackdetails ^= TRACK_SAVEAS_CSV;
    __energymonitor__filename = profFileName;
  }


  void ctrl_c_handler(int s) {
    __energymonitor__getNode->CloseINA231();
    exit(1);
  }

  void* energymonitor__startprofilingthread(void* threadid) {
    energymonitor__runfunction();
    return 0;
  }

  void energymonitor_filename(char * filename) {
    __energymonitor__filename = filename;
  }

  void track_CPUS(int trackcpu) {
    __energymonitor__trackflag = trackcpu;
  }
  
  void energymonitor__initialize() {
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = ctrl_c_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
    __energymonitor__trackflag = __ALL_CORES;
  }
  
  void energymonitor__init(int cores,float sleeptime) {
    energymonitor__initialize();
    energymonitor__settrackingcores(cores);
    track_CPUS(cores);
    energymonitor__setsleeptime(sleeptime);
  }
  
  void energymonitor__setsleeptime(float time) {
    int microsecsleep = (time * 1000000);
    __energymonitor__sleeptime = microsecsleep;
  }

  void energymonitor__trackpoweronly() {
    __energymonitor__trackdetails &= TRACK_SAVEAS_CSV;
  }

  void energymonitor__trackvoltage() {
    __energymonitor__trackdetails &= TRACK_SAVEAS_CSV;
    __energymonitor__trackdetails |=  TRACK_VOLTAGE;
  }

  void energymonitor__trackeverything() {
    __energymonitor__trackdetails |=  TRACK_VOLTAGE;
    __energymonitor__trackdetails |= TRACK_TEMPERATURE_AND_FREQ;
  }

  void energymonitor__startprofiling() {
    __energymonitor__keepprofiling = KEEP_PROFILE;
    myfile.open (__energymonitor__filename);
    time(&__energymonitor__start_time);
    pthread_create(&thread_id,NULL,energymonitor__startprofilingthread,(void*)1);
  }

  void energymonitor__settrackingcores(int cores) {
    __energymonitor__trackflag = cores;
    if (cores && __CORES_CPU4 == 0) {
      __energymonitor__bigTracking = 0;
      if( cores && __CORES_CPU5 ) {
	__energymonitor__bigTracking = 5;
	return;
      }
      if( cores && __CORES_CPU6 ) {
	__energymonitor__bigTracking = 6;
	return;
      }
      if( cores && __CORES_CPU7 ) {
	__energymonitor__bigTracking = 7;
	return;
      }
    }
  }

  void energymonitor__pauseprofiling() {
    __energymonitor__keepprofiling = PAUSE_PROFILE;
  }
  
  void energymonitor__unpauseprofiling() {
    __energymonitor__keepprofiling = KEEP_PROFILE;
  }

  void energymonitor__stopprofiling() {
    __energymonitor__keepprofiling = STOP_PROFILE;
    time_t endtime;
    time(&endtime);
    myfile.flush();
    myfile<<std::endl;
    myfile<<"\n Total Running time of the program is, "<<endtime-__energymonitor__start_time<<"secs"<<std::endl;
    myfile<<"\n Tracked Cores, " << __energymonitor__trackflag;
    myfile.flush();
    myfile.close();
    __energymonitor__trackflag = 0;
    pthread_join(thread_id,NULL);
  }
  

  int energymonitor__runfunction() {
    int i;
    time_t t;
    char NOW[20];
    float A7W, A15W, GPUW, MEMW;
    float A7V, A15V, GPUV, MEMV;
    __energymonitor__getNode = new GetNode();
	

    // enable the sensors
    if (__energymonitor__getNode->OpenINA231()) {
      std::cerr << "OpenINA231 error" << std::endl;
	  exit(1);
    } else {
      switch(__energymonitor__trackdetails) {
      case 0:
	myfile<<timeStr+powerStr;
	break;
      case 2:
	myfile<<timeStr+powerStr+voltStr;
	break;
      case 3:
	myfile<<timeStr+powerStr+voltStr+freqStr;
	break;
      case 4:
	myfile<<timeCsv+powerCsv;
	break;
      case 6:
	myfile<<timeCsv+powerCsv+voltCsv;
	break;
      case 7:
      default:
	  myfile<<timeCsv+powerCsv+voltCsv+freqCsv;
	break;
      }
    }
    myfile<<std::endl;
    
    while(__energymonitor__keepprofiling) {
      if(__energymonitor__keepprofiling  == KEEP_PROFILE) {
	__energymonitor__getNode->GetINA231();
	time(&t);
	strftime(NOW, sizeof(NOW), "%F %T", localtime(&t));
	myfile << NOW << ",";
	
	A7W = __energymonitor__getNode->kfcuW;
	A15W = __energymonitor__getNode->armuW;
	GPUW =  __energymonitor__getNode->g3duW;
	MEMW =  __energymonitor__getNode->memuW;
	myfile << std::setprecision(3) << A7W << "," << A15W << "," << MEMW << "," 
	       << GPUW << "," << std::setprecision(4) << A7W+A15W+MEMW+GPUW << ",";

	if(__energymonitor__trackflag & TRACK_VOLTAGE){
	  A7V = __energymonitor__getNode->kfcuV;
	  A15V = __energymonitor__getNode->armuV;
	  MEMV = __energymonitor__getNode->memuV;
	  GPUV = __energymonitor__getNode->g3duV;
	  myfile << std::setprecision(3) << A7V << "," << A15V << "," << MEMV << ","
	         << GPUV << ",";
	}

	if(__energymonitor__trackdetails & TRACK_TEMPERATURE_AND_FREQ){
	  myfile << __energymonitor__getNode->GetCPUCurFreq(0) << ",";
	  myfile << __energymonitor__getNode->GetCPUCurFreq(__energymonitor__bigTracking) << ","; 
	  myfile<< __energymonitor__getNode->GetGPUCurFreq() << ",";
	  
	  for (i = 0; i < NUMCORES/2; i++){
	    if(__energymonitor__trackflag & POS(i))
	      myfile << __energymonitor__getNode->GetCPUTemp(i) << ",";
	    else
	      myfile  << " 0 ,";  
	  }
	  myfile << __energymonitor__getNode->GetCPUTemp(NUMCORES/2) << ",";
	}

	myfile << std::endl;
      }	else {
	myfile <<"profile paused,"<<std::endl;
      }
      usleep(__energymonitor__sleeptime);
    } 
    return 0;
  }

}
