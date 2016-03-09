HOMEDIR := .
BIN= bin
SRC=examples
PTHREADSRC=pthread
OMPSRC=omp
LINEARSRC=singlethread
ENG=./energymodule
LIBS=-lm
INC=./include
C++FLAGS= -Wall -Werror -fpic
CFLAGS= -Wall -Werror -fpic
CC=gcc
C++ = g++
PTHREADFLG = -pthread
OMPFLG= -fopenmp
EMLKFLG = -lenergymodule
C11 = gcc -std=c11
C++11 = g++ -std=c++11
#################### STANDARD BENCHMARKS ##########
TASKBENCH= $(SRC)/$(OMPSRC)/taskbench_1.0

.PHONY: clean

all: test 

test: pthread omp linear
pthread: fileio fileiocpu pc
linear: simple
omp: mergesort matrixmultiplication

updateenergymodule: sharedobject profiler

sharedobject: $(ENG)/energylib.cpp $(ENG)/getnode.cpp  
	$(info ****************************** BUILDING ENERGY MODULE*********************** )
	$(C++11) -c $(C++FLAGS) $(PTHREADFLG) -I$(INC) $(ENG)/energylib.cpp $(ENG)/getnode.cpp 
	$(C++11) -shared -o libenergymodule.so energylib.o getnode.o
	sudo mv libenergymodule.so /usr/lib/
	rm -f *.o


profiler: $(ENG)/energylib.cpp $(ENG)/getnode.cpp $(ENG)/epprofiler.cpp
	$(C++11) $(PTHREADFLG)  -I$(INC) $(ENG)/energylib.cpp $(ENG)/getnode.cpp $(ENG)/epprofiler.cpp -o epprof
	mv epprof $(BIN)/

simple: $(SRC)/cexample.c $(SRC)/cppexample.cpp 	
	$(CC) -o $(BIN)/cexample -I$(INC) $(SRC)/cexample.c $(PTHREADFLG) $(EMLKFLG)
	$(C++) -o $(BIN)/cppexample -I$(INC) $(SRC)/cppexample.cpp $(PTHREADFLG) $(EMLKFLG)

fileio: $(SRC)/$(PTHREADSRC)/fileio.c 
	 $(C++) -o $(BIN)/fileio_pthread -I$(INC) $(SRC)/$(PTHREADSRC)/fileio.c $(PTHREADFLG) $(EMLKFLG)
fileiocpu: $(SRC)/$(PTHREADSRC)/fileiowithcpuaffinity.c
	$(C++) -o $(BIN)/fileiocpu_pthread -I$(INC) $(SRC)/$(PTHREADSRC)/fileiowithcpuaffinity.c $(PTHREADFLG) $(EMLKFLG)
pc: $(SRC)/$(PTHREADSRC)/producerconsumer.c
	$(CC) -o $(BIN)/pc_pthread -I$(INC) $(SRC)/$(PTHREADSRC)/producerconsumer.c $(PTHREADFLG) $(EMLKFLG)
mergesort: $(SRC)/$(OMPSRC)/mergesort.c
	$(CC) -o $(BIN)/ms_omp -I$(INC) $(SRC)/$(OMPSRC)/mergesort.c $(OMPFLG) $(EMLKFLG)
matrixmultiplication: $(SRC)/$(OMPSRC)/omp_mm.c
	$(CC) -o $(BIN)/mm_omp -I$(INC) $(SRC)/$(OMPSRC)/omp_mm.c $(OMPFLG) $(EMLKFLG)

clean:
	rm  -r ./$(BIN)/*
