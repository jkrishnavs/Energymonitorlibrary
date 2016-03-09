#include "energylib.h"
#include<cstring>
#include<iostream>
#include<string>


void helpmsg(){
  std::cout<<"Usage::"<<std::endl;
  std::cout<<"epprof -r [executablefile] [args to executable] -t [profile sampling time in sec] \n\t";
  std::cout<<"-g [profiler output filename default :epprof.prof] ";
  std::cout<<std::endl;
}


int main(int argc,char** argv){
  int noofargs=0;
  std::string exename,args[5];
  float timer;
  std::string filename="";
  int flag = 0;
  if(argc<2){
    helpmsg();
    flag=1;
  }
  int i=1;
  while(i<argc){
    if(strcmp(argv[i],"-r") == 0){
      i++;
      if(i<argc)
	exename = argv[i];
      i++;
      while(i<argc && argv[i][0] != '-'){
	args[noofargs++] = argv[i];
	i++;
      }
    }else if(strcmp(argv[i],"-t") == 0){
      i++;
      timer = std::stof(argv[i++]);
    }else if(strcmp(argv[i],"-g") == 0){
        i++;
	filename = argv[i++];
    }else{
      helpmsg();
      flag=1;
      break;
    }
  }

  if(flag)
    return flag;
  if(filename.empty())
    filename = "epprof.prof";

  int retVal;
  energymonitor__initialize();
  energymonitor__startprofiling();
  retVal = system(exename.data());
  energymonitor__stopprofiling();
 
}
