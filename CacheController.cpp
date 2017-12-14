

#include "CacheController.h"
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include "CacheConstants.h"
#include "Cache.h"
#include <queue>
#include "AtomicBusManager.h"
#include "CacheStats.h"
#include <stdlib.h>






/********************************************************************
 * Function:	CacheController Constructor
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
CacheController::CacheController(void)
{
}






/********************************************************************
 * Function:	CacheController Destructor
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
CacheController::~CacheController(void)
{
}






/********************************************************************
 * Function:	queuesEmpty
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
bool queuesEmpty(std::vector<Cache*> caches)
{
	bool allEmpty = true;
	for(int i = 0; i < caches.size(); i++)
	{
		if((*caches[i]).pendingJobs.size() != 0)
		{
			allEmpty = false;
		}
	}
	return allEmpty;
}







/********************************************************************
 * Function:	noJobs
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
bool noJobs(std::vector<Cache*> caches)
{
	bool allEmpty = true;
	for(int i = 0; i < caches.size(); i++)
	{
		if((*caches[i]).busy)
		{
			allEmpty = false;
		}
	}
	return allEmpty;

}




/********************************************************************
 * Function:	main
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
int main(int argc, char* argv[])
{
	
	int accessProcessorId = 0;
	char readWrite = ' ';
	unsigned long long address = 0;
	unsigned int threadId = 0;
	std::string line;

	if(argv[1] == NULL)
	{
	 printf("input format: ");
	 printf("./smp_cache <cache_size> <assoc> <block_size> <num_processors> \
	 <protocol> <trace_file> \n");
	 exit(0);
	}
	
	//printf("./smp_cache    <cache_size>     <assoc>      <block_size>       
	CacheConstants constants(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]),     
					   atoi(argv[4]),      atoi(argv[5]));
				 //  <num_processors>       <protocol>  
			
				 // <trace_file> \n");
	char* filename =    argv[6];

	
	//local var so don't have to do an object reference each time
	int numProcessors = constants.getNumProcessors();
	AtomicBusManager* bus;
	std::vector<Cache*> caches;
	CacheStats* stats[numProcessors];

	//keep track of all jobs that the processors have to do
	std::queue<CacheJob*> outstandingRequests; 
	
	
	printf("===== 506 Personal Information =====\n");
	printf("Justin Sayres\n");
	printf("jasayre2\n");	
	printf("ECE 506 (601)\n");

	//****************************************************//
	//**printf("===== Simulator configuration =====\n");**//
	//*******print out simulator configuration here*******//
	//****************************************************//
	printf("===== 506 SMP Simulator configuration =====\n");
	printf("L1_SIZE:  %d\n",constants.getNumCacheSize());
	printf("L1_ASSOC:  %d\n",constants.getNumLinesInSet());
	printf("L1_BLOCKSIZE:  %d\n",constants.getNumBytesInLine());
	printf("NUMBER OF PROCESSORS:  %d\n", constants.getNumProcessors());
	
	switch(constants.getProtocol())
	{
		case CacheConstants::MSI:
			printf("COHERENCE PROTOCOL:  MSI\n");
			break;
		case CacheConstants::MESI:
			printf("COHERENCE PROTOCOL:  MESI\n");
			break;
		case CacheConstants::DRAGON:
			printf("COHERENCE PROTOCOL:  Dragon\n");
			break;
		default:
			printf("Error\n");
			break;
	}
	printf("TRACE FILE:  %s\n",filename);


	
	if(filename == NULL)
	{
		printf("Error, no filename given");
		exit(0);
	}
	std::ifstream tracefile(filename);
	if(!tracefile)
	{
		printf("Error opening the tracefile, try again\n");
		exit(0);
	}
	
		
	while(getline(tracefile, line))
	{
		//so while there are lines to read from the trace,
		if(filename[7] == 'c')
			sscanf(line.c_str(), "%u %c %llx",&threadId, &readWrite, &address);
		else
			sscanf(line.c_str(), "%c %llx %u", &readWrite, &address, &threadId);

		outstandingRequests.push(new CacheJob((readWrite-32), address, threadId));
	}

	
	//Creating all of the caches and putting them into the caches vector
	for(int i = 0; i < constants.getNumProcessors(); i++)
	{
		stats[i] = new CacheStats();
		std::queue<CacheJob*> tempQueue;
		caches.push_back(new Cache(i, constants, &tempQueue, stats[i]));
	}

	//so now all queues are full with the jobs they need to run
	bus = new AtomicBusManager(constants, &caches);
	
	
	// While we are processing instructions or haven't passed off all requests to each processor
	while(!noJobs(caches) || !outstandingRequests.empty())
	{
		//time must first increment for the constants
		constants.tick();
		//then call for all the caches
		// if none of the processors are busy
		if(noJobs(caches))
		{
			#ifdef DEBUG
			printf("at cycle %llu we process a new job \n", constants.getCycle());
			#endif
			CacheJob* currJob = outstandingRequests.front();
			outstandingRequests.pop();
			int currThread = (*currJob).getThreadId();
			((*(caches[currThread])).pendingJobs).push(currJob);
			#ifdef DEBUG
			printf("Processor %i just got Job\n", currThread);
			#endif
		}
		
		for(int j = 0; j < numProcessors; j++)
		{
			#ifdef DEBUG
			if((*caches[j]).pendingJobs.size())
				printf("Processor %i has %li jobs\n", j, (*caches[j]).pendingJobs.size());
			#endif
			(*caches.at(j)).tick();
		}
		//then call the bus manager
		(*bus).tick();
	}
	
#ifdef DEBUG
	printf("finished at cycle %llu \n", constants.getCycle());
#endif
	
	//~ printf("Number of Ways %d\n", constants.getNumLinesInSet());
	//~ printf("Size of Block: %d\n", constants.getNumBytesInLine());
	//~ printf("Size of Cache: %d\n", constants.getNumCacheSize());
	//~ printf("SNumber of Sets: %d\n", constants.getNumSets());
	
	for(int j = 0; j < numProcessors; j++)
		(*(caches[j])).printStats();
		
	for(int i = 0; i < numProcessors; i++)
		delete caches[i];
		
	tracefile.close();
}

