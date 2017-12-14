#ifndef CACHESTATS_H
#define CACHESTATS_H





class CacheStats
{
public:
	unsigned long long numHit;
	unsigned long long numMiss;		

	unsigned long long numExclusiveToModifiedTransitions;
	unsigned long long numReads;				// 01. Number of read transactions the cache has received.               
	unsigned long long numReadMisses;			// 02. Number of read misses the cache has suffered.   
	unsigned long long numWrites;				// 03. Number of write transactions the cache has received. 
	unsigned long long numWriteMisses;			// 04: Number of write misses the cache has suffered.    
					float missRate;				// 05. Total miss rate (rounded to 2 decimals, should use percentage format)  
	unsigned long long numWriteBacks;			// 06. Number of dirty blocks written back to the main memory:  dirty blocks written back to the main memory         
	unsigned long long numCacheShare;			// 07. Number of cache-to-cache transfers (from the requestor cache perspective, i.e. how many lines this cache has received from other peer caches):  how many lines this cache has received from other peer caches
	unsigned long long numInterventions;		// 09. Number of interventions (refers to the total number of times that E/M transits to Shared states. Hint: There are two shared states in Dragon. See Chapter 8 for details): E/M state to Shared
	unsigned long long numInvalidations;		// 10. Number of invalidations (any State‐>Invalid)    Any State to Invalid
	unsigned long long numEvict;				// numEvictions??????
	unsigned long long numFlush;				// 11. Number of flushes to the main memory.  To main Memory????
	unsigned long long numMainMemoryUses;		// 11. Is this Evictions/writebacks?
	unsigned long long numBusRequests;			// 12. Does this include BUSRD aand BUSRDX???
	unsigned long long numBusReadX;				// 12. Number of issued BusRdX transactions. 
	unsigned long long numWriteShared;
unsigned long long numWriteHits;
unsigned long long numReadHits;
	CacheStats(void);
	~CacheStats(void);
};

#endif

                   

                          

                









           

            

