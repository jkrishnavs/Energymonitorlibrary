#include<cstdio>
#include<pthread.h>
#include<cstdlib>
#include<time.h>
#include "energylib.h"

#define BLOCKOPS 100000
#define TOTALOPS 800000
#define WRITELIMIT 3
#define NUM_THREADS 4
#define NUM_JOBS 8


pthread_mutex_t oddvar=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t evenvar = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t curiter= PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t newwork;
pthread_cond_t workfinished;

int * ODDHASH;
int * EVENHASH;

#define BLOCK_FREE 0
#define ENQEUE_HAPPENING 1
#define ENQUE_DONE 2
#define PROCESSING 3



int globaldata[TOTALOPS*2];
int jobstatus[NUM_JOBS];
int finishedparsing,finishedprocessing;

int oddsize,evensize;

void fileread(char* filename, int *data,int size){
  FILE* fp = fopen(filename,"r");
  int i;
  for(i=0;i<size;i++){
    fscanf(fp,"%d %d",&data[2*i],&data[2*i+1]);
  }
  
  fclose(fp);
}


int readodd(int data){
  int i;
  int retval= 0;
  pthread_mutex_lock(&oddvar);
  
  for(i=0;i<oddsize;i++){
    if(ODDHASH[i] == data)
      retval = 1;
  }
  pthread_mutex_unlock(&oddvar);
  return retval;
}


int readeven(int data){
  int i;
  int retval= 0;
  pthread_mutex_lock(&evenvar);
  
  for(i=0;i<evensize;i++){
    if(EVENHASH[i] == data)
      retval = 1;
  }
  pthread_mutex_unlock(&evenvar);
  return retval;
}


int writeodd(int data){
  int i;
  int retval= 0;
  pthread_mutex_lock(&oddvar);
  
  for(i=0;i<oddsize;i++){
    if(ODDHASH[i] == data)
      retval = -1;
  }
  
  if(retval){
    ODDHASH[i++] = data;
  }
  if(i > 4999)
    printf("Error array out of bounds ");
  pthread_mutex_unlock(&oddvar);
  return retval;
}

int writeeven(int data){
  int i;
  int retval= 0;
  pthread_mutex_lock(&evenvar);
  
  for(i=0;i<evensize;i++){
    if(EVENHASH[i] == data)
      retval = 1;
  }
  
  if(retval){
    EVENHASH[i++] = data;
  }
  if(i > 4999)
    printf("Error array out of bounds ");
  pthread_mutex_unlock(&evenvar);
  return retval;
}



void processing(int* data,int size){

  int i,j;
  
  for(i=0;i<size;i++){
    int number = data[2*i+1];
    int flag = data[2*i];
    for(j=0;j<BLOCKOPS/10;j++);
    if( size < j)
      continue;


    if(flag == 0){
      if(number%2 ==0)
	readeven(number);
      else
	readodd(number);
    }
    else{
      if(number%2 ==0)
	writeeven(number);
      else
	writeodd(number);
    }
  }
}






void filewrite(char* filename, int* data, int size){
  FILE* fp = fopen(filename,"r");
  int i;
 
  for(i=0;i<size;i++){
    fprintf(fp,"%d %d\n",data[2*i],data[2*i+1]);
  }
  
  fclose(fp);
}


int randomnumber(int limit){
  int ret = rand()%limit;
  return ret;
}

char *filename="test.txt";


void* ReadFromFile(void* t){
  
  printf("Start thread\n");

  int i;
  long tid = (long)t;
  FILE* fp= fopen(filename,"r");
  int flag;

  int curparse;
  pthread_mutex_lock(&curiter);
  curparse = finishedparsing;
  pthread_mutex_unlock(&curiter);
  while(curparse<(NUM_JOBS-1)){
    flag = -1;
    pthread_mutex_lock(&curiter);
    if(finishedparsing< (NUM_JOBS-1)){
      for(i=0;i<NUM_JOBS;i++){
	if(jobstatus[i] == BLOCK_FREE && flag == -1){
	  finishedparsing++;
	  flag = i;
	  jobstatus[i] = ENQEUE_HAPPENING;
	  curparse = finishedparsing;
	}
      }
    }
    pthread_mutex_unlock(&curiter);
    if(flag != -1){

      int iterator = flag*BLOCKOPS*2;
      int skip =0;int skipdata;
      for(skip=0;skip<iterator;skip++){
	fscanf(fp,"%d",&skipdata);	
      }
      int *data = &(globaldata[iterator]);
      for(i=0;i<BLOCKOPS;i++){
	fscanf(fp,"%d %d",&data[iterator++],&data[iterator++]);
      }
      pthread_mutex_lock(&curiter);
      jobstatus[flag] = ENQUE_DONE;
      printf("file copy finished for queue id %d by tid %ld\n",flag,tid); 
      pthread_mutex_unlock(&curiter);   
    }
    else{
      pthread_mutex_lock(&curiter);
      curparse = finishedparsing;
      pthread_mutex_unlock(&curiter);
    }
  }
  printf("file copy finished %ld\n",tid);
  return NULL;
} 

void* BusyWork(void* t){
 
  int myiter;
  long tid = (long)t;
  struct timespec timeToWait;
  int iterator;
  int i;
  
  pthread_mutex_lock(&curiter);
  myiter = finishedprocessing;
  pthread_mutex_unlock(&curiter);


  while(myiter < NUM_JOBS-1){
    iterator = -1;
    pthread_mutex_lock(&curiter);
    for(i=0;i<NUM_JOBS;i++){
      if(jobstatus[i] == ENQUE_DONE && iterator == -1){
	finishedprocessing++;
	iterator = i;
	jobstatus[i] = PROCESSING;
      }
    }
    myiter = finishedprocessing;
    pthread_mutex_unlock(&curiter);
    
    if(iterator != -1){
      printf("Dequeue val %d by thread %ld \n",iterator,tid);
      int startaddr = iterator*BLOCKOPS*2;
      int* data = globaldata+(startaddr);
      processing(data,BLOCKOPS);
    }
  }
  
  printf("Returning from work thread id %ld\n",tid);
  return(NULL);

}

void DoWork(){
  pthread_t thread[NUM_THREADS];
  int rc;
  long t;

  int i;
  for(i=0;i<NUM_JOBS;i++){
    jobstatus[i] = BLOCK_FREE;
  }

  /* Initialize and set thread detached attribute */
  int cores = __CORES_CPU0|__CORES_CPU1|__CORES_CPU4|__CORES_CPU5;
  energymonitor__init(cores,1);
  energymonitor__startprofiling();
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);  
  for(t=0; t<NUM_THREADS; t++) {
    //    printf("Main: creating thread %ld\n", t);
    if(t%2 == 1)
      rc = pthread_create(&thread[t], &attr, BusyWork, (void *)t); 
    else
      rc = pthread_create(&thread[t], &attr, ReadFromFile, (void *)t);     
    if (rc) {
      printf("ERROR return code from pthread_create is %d\n", rc);
      exit(-1);
    }
  }
  for(t=0;t<NUM_THREADS;t++){
    pthread_join(thread[t],NULL);
  }
  energymonitor__stopprofiling();
}

void Initial(){
  int i=0;
  FILE* fp;
  fp = fopen(filename,"w");
  for(i=0;i<TOTALOPS;i++){
    int val = randomnumber(10000);
    int toss = randomnumber(100);
    if(toss<WRITELIMIT){
      fprintf(fp,"1  %d\n",val);
    }else
      fprintf(fp,"0 %d\n",val);
      
  }
  finishedprocessing = -1;
  finishedparsing = -1;
}


int main(){

  srand(0);
  
  ODDHASH = (int*) malloc (5000* sizeof(int));
  EVENHASH = (int*) malloc (5000* sizeof(int));
  Initial();
  DoWork();
  return 0;
}
