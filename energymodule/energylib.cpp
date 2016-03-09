// coded by chan at 2014-09-04

#include "energylib.h"
#include "getnode.h"
#include <fstream>
#include <iostream> // cout, endl
#include <cstdlib>	// atoi
#include <csignal>
#include <unistd.h>	// sleep
#include <iomanip>	// cout format


extern "C" {
  /*** COMMON VARIABLES *************/ 
#define PAUSE_PROFILE 2
#define KEEP_PROFILE 1
#define STOP_PROFILE 0

#define POS(i) (1<<i)
  float __energymonitor__sleeptime = 5.0;
  int __energymonitor__keepprofiling = STOP_PROFILE;


  GetNode *__energymonitor__getNode;
  time_t __energymonitor__start_time;
  pthread_t thread_id;
  std::ofstream myfile;
  int __energymonitor__trackflag;

  /*                                                                                                                                                                                                       
      float __energymonitor__peakBigPowerConsumption;                                                                                                                                                        
      float __energymonitor__peakLittlePowerConsumption;                                                                                                                                                     
      float __energymonitor__peakCPUPowerConsumption;                                                                                                                                                        
                                                                                                                                                                                                             
      int __energymonitor__totalCPUTime[8];                                                                                                                                                                  
    float __energymonitor__overallEnergyConsumption[4];                                                                                                                                                      
  */
  std::string __energymonitor__filename="prof.txt";

  /*** COMMON VARIABLES *************/ 


  void ctrl_c_handler(int s) {
    __energymonitor__getNode->CloseINA231();
    exit(1);
  }

  void* energymonitor__startprofilingthread(void* threadid){
    energymonitor__runfunction();
    return 0;
  }

  void energymonitor_filename(char * filename){
    __energymonitor__filename = filename;
  }

  void track_CPUS(int trackcpu){
    __energymonitor__trackflag = trackcpu;
  }
  
  void energymonitor__initialize(){
    
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = ctrl_c_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
    __energymonitor__trackflag = __ALL_CORES;
  }
  
  void energymonitor__init(int cores,float sleeptime){
    energymonitor__initialize();
    energymonitor__settrackingcores(cores);
    track_CPUS(cores);
    energymonitor__setsleeptime(sleeptime);
  }
  
  void energymonitor__setsleeptime(float time){
    int microsecsleep = (time * 1000000);
    __energymonitor__sleeptime = microsecsleep;
  }

  void energymonitor__startprofiling(){
  __energymonitor__keepprofiling = KEEP_PROFILE;
  myfile.open (__energymonitor__filename);
  time(&__energymonitor__start_time);
  pthread_create(&thread_id,NULL,energymonitor__startprofilingthread,(void*)1);
  
  }
  

  void energymonitor__settrackingcores(int cores){
    __energymonitor__trackflag = cores;
  }

  void energymonitor__pauseprofiling(){
    __energymonitor__keepprofiling = PAUSE_PROFILE;
  }
  
  void energymonitor__unpauseprofiling(){
    __energymonitor__keepprofiling = KEEP_PROFILE;
  }

  void energymonitor__stopprofiling(){
    __energymonitor__keepprofiling = STOP_PROFILE;
    time_t endtime;
    time(&endtime);
    myfile<<"Total Running time of the program is::"<<endtime-__energymonitor__start_time<<"secs"<<std::endl;
    myfile.close();
    __energymonitor__trackflag = 0;
    pthread_join(thread_id,NULL);
  }
  

  int energymonitor__runfunction()
  {
    
    int i;
    time_t t;
    char NOW[20];
    float A7W, A15W, GPUW, MEMW;
    __energymonitor__getNode = new GetNode();
	

    // enable the sensors
    if (__energymonitor__getNode->OpenINA231()) {
      std::cerr << "OpenINA231 error" << std::endl;
	  exit(1);
    }else{
      myfile<<"profile dump\n";
      myfile<< "Time|CPU0_Freq(MHz)|CPU1_Freq|CPU2_Freq|CPU3_Freq|CPU4_Freq|CPU5_Freq|CPU6_Freq|CPU7_Freq|GPU_Freq|CPU0_Temp(C)|CPU1_Temp|CPU2_Temp|CPU3_Temp|GPU_Temp|A7_W|A15W|MEM_W|GPU_W|Watt_Sum" << std::endl;

    }
    
    while(__energymonitor__keepprofiling) {
      if(__energymonitor__keepprofiling  == KEEP_PROFILE){
	__energymonitor__getNode->GetINA231();
	time(&t);
	strftime(NOW, sizeof(NOW), "%F %T", localtime(&t));
	myfile << NOW << "|";
	
	for (i = 0; i < NUMCORES; i++){
	  if(__energymonitor__trackflag & POS(i))
	    myfile << __energymonitor__getNode->GetCPUCurFreq(i) << "|";
	  else
	    myfile  << " 0 |";  
	}
	myfile<< __energymonitor__getNode->GetGPUCurFreq() << "|";
	
	for (i = 0; i < NUMCORES/2; i++){
	  if(__energymonitor__trackflag & POS(i))
	    myfile << __energymonitor__getNode->GetCPUTemp(i) << "|";
	  else
	    myfile  << " 0 |";  
	}
	myfile << __energymonitor__getNode->GetCPUTemp(NUMCORES/2) << "|";
	
	A7W = __energymonitor__getNode->kfcuW;
	A15W = __energymonitor__getNode->armuW;
	GPUW =  __energymonitor__getNode->g3duW;
	MEMW =  __energymonitor__getNode->memuW;
	
	myfile << std::setprecision(3) << A7W << "|" << A15W << "|" << MEMW << "|" 
	       << GPUW << "|" << std::setprecision(4) << A7W+A15W+MEMW+GPUW << std::endl;
      }	else{
	myfile <<"profile paused"<<std::endl;
      }
      usleep(__energymonitor__sleeptime);
    } 

    return 0;
  }

}
