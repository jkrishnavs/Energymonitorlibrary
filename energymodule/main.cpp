// coded by chan at 2014-09-04

#define NUMCORES	8

#include "getnode.h"
#include <string> 	// string
#include <iostream> // cout, endl
#include <cstdlib>	// atoi
#include <csignal>
#include <unistd.h>	// sleep
#include <ctime>
#include <iomanip>	// cout format


GetNode* getNode;

void ctrl_c_handler(int s) {
	getNode->CloseINA231();
	exit(1);
}

int main(int argc, char* argv[])
{
	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = ctrl_c_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);

	int sleeptime = 5;
	int i;

	time_t t;
	char NOW[20];
	float A7W, A15W, GPUW, MEMW;

	if (argc > 1) 
		if ((sleeptime = atoi(argv[1])) == 0) {
			sleeptime = 5;
		}

	getNode = new GetNode();
	
	//	std::cout << "Time|CPU0_Freq(MHz)|CPU1_Freq|CPU2_Freq|CPU3_Freq|CPU4_Freq|CPU5_Freq|CPU6_Freq|CPU7_Freq|GPU_Freq|CPU0_Temp(C)|CPU1_Temp|CPU2_Temp|CPU3_Temp|GPU_Temp|A7_W|A15W|MEM_W|GPU_W|Watt_Sum" << std::endl;

	// enable the sensors
	if (getNode->OpenINA231()) {
	  std::cerr << "OpenINA231 error" << endl;
		exit(1);
	}
	else {
	  std::cerr << "This loops forever until press <Ctrl-C>!" << endl;
	}

	while(1) {
		getNode->GetINA231();
		time(&t);
		strftime(NOW, sizeof(NOW), "%F %T", localtime(&t));
		cout << NOW << "|";

		for (i = 0; i < NUMCORES; i++)
			cout << getNode->GetCPUCurFreq(i) << "|";
		cout << getNode->GetGPUCurFreq() << "|";

		for (i = 0; i < NUMCORES/2; i++)
			cout << getNode->GetCPUTemp(i) << "|";
		cout << getNode->GetCPUTemp(NUMCORES/2) << "|";

		A7W = getNode->kfcuW;
		A15W = getNode->armuW;
		GPUW = getNode->g3duW;
		MEMW = getNode->memuW;
		
		cout << setprecision(3) << A7W << "|" << A15W << "|" << MEMW << "|" << GPUW << "|" << setprecision(4) << A7W+A15W+MEMW+GPUW << endl;

		sleep(sleeptime);
	} 

	return 0;
}
