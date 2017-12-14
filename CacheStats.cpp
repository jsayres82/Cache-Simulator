#include "CacheStats.h"





/********************************************************************
 * Function:	CacheStats Constructor
 * Inputs:		None
 * Outputs:		None
 * Description: Records information such as:  number of hits,  number 
 * 				of misses, number of flushes, false sharing somehow?
 * 
 * ******************************************************************/
CacheStats::CacheStats(void)
{
	
	numHit	= 0;	
	numMiss = 0;
	numExclusiveToModifiedTransitions = 0;
	numReads = 0;
	numReadMisses = 0;
	numWrites = 0;
	numWriteMisses = 0;
	missRate = 0;
	numWriteBacks = 0;
	numCacheShare = 0;
	numInterventions = 0;
	numInvalidations = 0;
	numEvict = 0;
	numFlush = 0;
	numMainMemoryUses = 0;
	numBusRequests = 0;
	numBusReadX = 0;
	numWriteShared = 0;
	numWriteHits = 0;
	numReadHits = 0;
}




/********************************************************************
 * Function:	CacheStats Destructor
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
CacheStats::~CacheStats(void)
{
}
