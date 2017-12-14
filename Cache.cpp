#include "Cache.h"
#include "CacheConstants.h"
#include "CacheController.h"	
#include "CacheSet.h"
#include "vector"
#include "CacheJob.h"
#include "queue"
#include "BusRequest.h"
#include "CacheLine.h"



/********************************************************************
 * Function:	Constructor
 * Inputs:		processor#, Cache Settings,CacheJob Queue, Cache Stats 
 * Outputs:		New Cache
 * Description:  Initializes a new cache with the appropriate number 
 * 				of sets.Manages generating busrequests, handles 
 * 				processing of all the processor's requests, and 
 * 				maintains its own LRU cache
 * 
 * ******************************************************************/
Cache::Cache(int pId, CacheConstants consts, std::queue<CacheJob*>* jobQueue, CacheStats* st)
{

	cacheConstants = consts;
	//make a vector of the CacheSet 
	localCache.resize(cacheConstants.getNumSets());
	for(int i = 0; i < cacheConstants.getNumSets(); i++)
	{
		localCache[i] = new CacheSet(&consts, (long unsigned int)i);	
	}
	//~ ShowCache();
	processorId = pId;
	pendingJobs = *jobQueue;
	currentJob = NULL;
	busRequest = NULL;
	haveBusRequest = false;
	busy = false;
	busRequestBeingServiced = false;
	startServiceCycle = 0;
	jobCycleCost = 0;
	stats = st;
}




/********************************************************************
 * Function:	setPId
 * Inputs:		processor#
 * Outputs:		None
 * Description:  assigns the appropriate id based on the index of the 
 * 				cache
 * 
 * ******************************************************************/
void Cache::setPId(int pid)
{
	processorId = pid;
}



/********************************************************************
 * Function:	ShowCache
 * Inputs:		None
 * Outputs:		None
 * Description:  Prints all lines and ways of the cache
 * 
 * ******************************************************************/
void Cache::ShowCache()
{
	char tmpstr[256];
	sprintf(tmpstr,"Line\t");
	printf("Cache %i: \n", processorId);
	//~ for (int i = 0; i < cacheConstants.getNumLinesInSet(); ++i)
		//~ sprintf(tmpstr, "%sTAG::INDEX-STATE-LASTUSE\t",tmpstr);
	//~ printf("%s\n",tmpstr);
	for(int i = 0; i < cacheConstants.getNumSets(); i++){
		printf("%i\t",i);
		localCache[i]->getLRU();
	}
}



/********************************************************************
 * Function:	ShowLine
 * Inputs:		Line of Cache to display
 * Outputs:		None
 * Description:  prints out the cache line with all ways
 * 
 * ******************************************************************/
void Cache::ShowLine(unsigned long int line)
{
	unsigned long long jobAddr = (*currentJob).getAddress();
	char tmpstr[256];
	unsigned long int set = 0;
	unsigned long int tag = 0;
	decode_address((*currentJob).getAddress(), &set, &tag);
	sprintf(tmpstr,"Line\t");
	printf("Cache %i: %llx\n", processorId, jobAddr);
	//~ for (int i = 0; i < cacheConstants.getNumLinesInSet(); ++i)
			//~ sprintf(tmpstr, "%sTAG::INDEX-STATE-LASTUSE\t",tmpstr);
	//~ printf("%s\n",tmpstr);
	printf("%lu\t",line);
	localCache[line]->getLRU();
	
}








/********************************************************************
 * Function:	decode_address
 * Inputs:		memory location address, pointer to variables for 
 *              index and tag
 * Outputs:		None
 * Description:  Given an address, and two int*, set the pointer values 
 * 					to the set number and the tag for the address
 * 
 * ******************************************************************/
void Cache::decode_address(unsigned long long address, unsigned long int * whichSet, unsigned long int * tag)
{
	unsigned long int  numSetBits = cacheConstants.getNumSetBits();
	unsigned long int  numBytesBits = cacheConstants.getNumBytesBits();
    unsigned long int  numTagBits = cacheConstants.getTagMask();

	unsigned long int  currTag = (address  >> numBytesBits);// & ((1 << numSetBits)-1);
	unsigned long int  currSet = ((address >> ((numBytesBits))) & numTagBits);
	//~ ulong calcIndex(ulong addr)  { return ((addr >> log2Blk) & tagMask);}
	//~ printf("Address: %llx:   Tag %lx:  Set %lx:\n",address, currTag, currSet);
	//~ printf("Cache %i:Address %llx: Tag %lx: Line %lx:    \n", processorId, address, currTag, currSet);
	//~ 
	*whichSet = currSet;
	*tag = currTag;
	if((currTag == 0))
		printf("\n\n\nBIG PROBLEM\n\n\n");
}






/********************************************************************
 * Function:	getTotalMemoryCost
 * Inputs:		Set and Tag of cache to determine transaction time
 * Outputs:		the number of cycles of the transaction
 * Description:   Determines time for transactino
 * 
 * ******************************************************************/
unsigned long long Cache::getTotalMemoryCost(unsigned long int  set, unsigned long int  tag)
{
	// We have to go to memory so how many cycles does it take
	unsigned long long result;
	
	if((tag == 0))
		printf("\n\n\nBIG PROBLEM\n\n\n");
		
		
	if(cacheConstants.getProtocol() == CacheConstants::MSI)
		result = cacheConstants.getMemoryResponseCycleCost();
	else if(cacheConstants.getProtocol() == CacheConstants::MESI)
		result = cacheConstants.getCacheResponseCycleCost();
	else if(cacheConstants.getProtocol() == CacheConstants::DRAGON)	
		result = cacheConstants.getCacheResponseCycleCost();
	
	
		CacheSet* currSet = localCache[set]; 
		if (!(*currSet).hasLine(tag))
	{
		//only pay the cycle cost if we have to evict a line with data we care about
		if ((*currSet).isFull() && (*currSet).evictLineModified())
		{
			result = result*2;
			(*stats).numFlush++;
			printf("flush from an evict modify \n");
		}
	}
	//~ printf("Current Set %x: Tag %x: \n", set, tag);	

	// if we don't have the line then we need add it
	
	//~ if (!(*currSet).hasLine(tag))
	//~ {
		//~ 
			//~ if((*currSet).hasInvalidLine())
			//~ {
				//~ printf("Has invalid line\n");
				//~ (*currSet).evictLRULineInvalid();
			//~ }
			//~ else if((*currSet).allLinesFull())
			//~ {
				//~ if((*currSet).evictLineModified())
				//~ {
					//~ result = result*2;
					//~ 
				//~ #ifdef DEBUG
					//~ printf("flush from an evict modify \n");
				//~ #endif
					//~ (*currSet).evictLRULine();
					//~ (*stats).numWriteBacks++;	
				//~ }
				//~ else if((*currSet).evictLineShared())
				//~ {
					//~ (*currSet).evictLRULine();
				//~ }
				//~ else
				//~ {
					//~ printf("should never happen\n");
					//~ (*currSet).evictLRULine();
				//~ }
			//~ }
		
	//~ }
	return result;
}

/********************************************************************
 * Function:	lineValid
 * Inputs:		State to compare to
 * Outputs:		Whether the block state matches
 * Description:  For a given state, see if the line the current job we 
 * 				are working on is in that state
 * 
 * ******************************************************************/
bool Cache::isLineValid()
{
	unsigned long int  set = 0;
	unsigned long int  tag = 0;
	
	decode_address((*currentJob).getAddress(), &set, &tag);
	
	if((tag == 0))
		printf("\n\n\nBIG PROBLEM\n\n\n");
		
	if((localCache[set] != NULL) && (*localCache[set]).lineValid(tag))
	{
			return true;
	}
	
	return false;
}



/********************************************************************
 * Function:	lineInState
 * Inputs:		State to compare to
 * Outputs:		Whether the block state matches
 * Description:  For a given state, see if the line the current job we 
 * 				are working on is in that state
 * 
 * ******************************************************************/
bool Cache::lineInState(CacheLine::State state)
{
	unsigned long int  set = 0;
	unsigned long int  tag = 0;
	
	decode_address((*currentJob).getAddress(), &set, &tag);
	
	if((tag == 0))
		printf("\n\n\nBIG PROBLEM\n\n\n");
		
	if((localCache[set] != NULL) && (*localCache[set]).hasLine(tag))
	{
		CacheLine* theLine = (*localCache[set]).getLine(tag);
		if((*theLine).getState() == state)
			return true;
	}
	
	return false;
}





/********************************************************************
 * Function:	setLineState
 * Inputs:		State to transition cache block to
 * Outputs:		None
 * Description:  Decodes address, checks to make sure it is valid then
 * 				assigns the new state
 * 
 * ******************************************************************/
void Cache::setLineState(CacheLine::State state)
{
	unsigned long int  set = 0;
	unsigned long int  tag = 0;
	
	decode_address((*currentJob).getAddress(), &set, &tag);
	if((tag == 0))
		printf("\n\n\nBIG PROBLEM\n\n\n");
	if((localCache[set] != NULL) && (*localCache[set]).hasLine(tag))
	{
		CacheLine* theLine = (*localCache[set]).getLine(tag);
		(*theLine).setState(state);
	}
}





/********************************************************************
 * Function:	handleWriteModified
 * Inputs:		None
 * Outputs:		Whether the or not a write on modified state took place
 * Description:  Checks to see if block is modified and if so adds 
 *              cycle time for cache hit
 * 
 * ******************************************************************/
bool Cache::handleWriteModified()
{
	if(lineInState(CacheLine::modified))
	{
		//so we can service this request ez
		startServiceCycle = cacheConstants.getCycle();
		jobCycleCost = cacheConstants.getCacheHitCycleCost();
		busy = true;
		haveBusRequest = false;
		(*stats).numHit++;
		(*stats).numWriteHits++;
	#ifdef DEBUG
		printf("cache %d just got a cache HIT on a PrWr request for address %llx in MODIFIED state at cycle %llu \n", 			processorId, (*currentJob).getAddress(), cacheConstants.getCycle());
	#endif

		return true;
	}
	return false;
}






/********************************************************************
 * Function:	handleWriteExclusive
 * Inputs:		None
 * Outputs:		Whether the or not a write on exclusive state took place
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
bool Cache::handleWriteExclusive()
{
	if(lineInState(CacheLine::exclusive))
	{
		//so have to change the line state to modified
		//but it's a hit so no need for bus
		startServiceCycle = cacheConstants.getCycle();
		jobCycleCost = cacheConstants.getCacheHitCycleCost();
		busy = true;
		haveBusRequest = false;
		(*stats).numWriteHits++;
		(*stats).numHit++;
		(*stats).numExclusiveToModifiedTransitions++;

		//set it to modified state
		setLineState(CacheLine::modified);
	#ifdef DEBUG
		printf("cache %d just got a cache HIT on a PrWr request for address %llx in EXCLUSIVE state, so changed to MODIFIED state at cycle %llu\n",			processorId, (*currentJob).getAddress(), cacheConstants.getCycle());
	#endif
		return true;
	}
	return false;
}






/********************************************************************
 * Function:	handleWriteSharedInvalid
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
void Cache::handleWriteSharedInvalid()
{
	haveBusRequest = true;
	busy = true;
	unsigned long int set = 0;
	unsigned long int tag = 0;
	decode_address((*currentJob).getAddress(), &set, &tag);
	if((tag == 0))
		printf("\n\n\nBIG PROBLEM\n\n\n");
		
	if((localCache[set] != NULL) && (*localCache[set]).hasLine(tag))
	{
		if(lineInState(CacheLine::shared))
		{
			(*stats).numWriteShared++;	
			(*stats).numWriteHits++;
		}
	}

	jobCycleCost = getTotalMemoryCost(set, tag);
	busRequest = new BusRequest(BusRequest::BusRdX, set, tag,
		jobCycleCost, (*currentJob).getAddress());
	setLineState(CacheLine::modified);
	
#ifdef DEBUG
	printf("cache %d just got a cache MISS(or was SHARED) on a PrWr request for address %llx at cycle %llu \n", processorId, (*currentJob).getAddress(), cacheConstants.getCycle());
#endif
}







/********************************************************************
 * Function:	handleWriteRequestMESI
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
void Cache::handleWriteRequestMESI()
{

	//only difference here is that we have to case if we're in exclusive state
	if(handleWriteModified())
	{
		return;
	}
	else if(handleWriteExclusive())
	{
		return;
	}
	else
	{
		handleWriteSharedInvalid();
	}	
}







/********************************************************************
 * Function:	handleWriteRequestMSI
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
void Cache::handleWriteRequestMSI()
{

	if(handleWriteModified())
	{
		return;
	}
	else
	{

		handleWriteSharedInvalid();
	}
}









/********************************************************************
 * Function:	handleReadHit
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
bool Cache::handleReadHit()
{
	if(lineInState(CacheLine::modified) || lineInState(CacheLine::shared) || lineInState(CacheLine::exclusive) || lineInState(CacheLine::owned))
	{
		//cache hit
		haveBusRequest = false;
		busy = true;
		startServiceCycle = cacheConstants.getCycle();
		jobCycleCost = cacheConstants.getCacheHitCycleCost();
		(*stats).numReadHits++;
		(*stats).numHit++;
#ifdef DEBUG
	printf("cache %d just got a cache HIT on a PrRd request for address %llx at cycle %llu \n",			processorId, (*currentJob).getAddress(), cacheConstants.getCycle());
#endif
		return true;
	}
	return false;
}








/********************************************************************
 * Function:	handleReadMiss
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
void Cache::handleReadMiss()
{
	//so we need to issue a request for the line
	haveBusRequest = true;
	busy = true;
	unsigned long int set = 0;
	unsigned long int tag = 0;
	
	decode_address((*currentJob).getAddress(), &set, &tag);
	
	if((tag == 0))
		printf("\n\n\nBIG PROBLEM\n\n\n");
		
	busRequest = new BusRequest(BusRequest::BusRd, set, tag,
	cacheConstants.getMemoryResponseCycleCost(), (*currentJob).getAddress());

	(*stats).numMiss++;
	
#ifdef DEBUG
	printf("cache %d just got a cache MISS on a PrRd request for address %llx at cycle %llu \n",		processorId, (*currentJob).getAddress(), cacheConstants.getCycle());
#endif
}









/********************************************************************
 * Function:	handleReadRequestMESI
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
void Cache::handleReadRequestMESI()
{
	if(isLineValid() == lineInState(CacheLine::invalid))
		printf("\n\n\nBIG PROBLEM\n\n\n");
		
		if(isLineValid())
			handleReadHit();
		else
		{
			handleReadMiss();
			 (*stats).numReadMisses++;
		 }
	//~ if(handleReadHit())
	//~ {
		//~ return;
	//~ }
	//~ else
	//~ {
		//~ handleReadMiss();
	//~ }
}







/********************************************************************
 * Function:	handleReadRequestMSI
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
void Cache::handleReadRequestMSI()
{
		if(isLineValid())
			handleReadHit();
		else
		{
			handleReadMiss();
			 (*stats).numReadMisses++;
		 }
	//~ if(handleReadHit())
	//~ {
		//~ return;
	//~ }
	//~ else
	//~ {
		//~ handleReadMiss();
	//~ }
}








/********************************************************************
 * Function:	handleRequest
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  process a cache request, and ask for bus usage if 
 * 				necessary
 * 
 * ******************************************************************/
void Cache::handleRequest()
{
	if (!busy)
	{
		//so there are still jobs and we're not doing one right now
		if(!pendingJobs.empty())
		{
			currentJob = pendingJobs.front();
			pendingJobs.pop();
		#ifdef DEBUG
			printf("lets make a job for cache %d at cycle %llu \n", processorId, cacheConstants.getCycle());
		#endif
			if((*currentJob).isWrite())
			{
				(*stats).numWrites++;
						
				if(cacheConstants.getProtocol() == CacheConstants::MSI)
				{
					handleWriteRequestMSI();
					return;
				}
				if(cacheConstants.getProtocol() == CacheConstants::MESI)
				{
					handleWriteRequestMESI();
					return;
				}
				if(cacheConstants.getProtocol() == CacheConstants::DRAGON)
				{
					if(lineInState(CacheLine::sharedClean))
					{
						
						haveBusRequest = true;
						busy = true;
						unsigned long int  set = 0;
						unsigned long int  tag = 0;
						decode_address((*currentJob).getAddress(), &set, &tag);
						if((tag == 0))
							printf("\n\n\nBIG PROBLEM\n\n\n");

						unsigned long long memoryCost = cacheConstants.getCacheHitCycleCost();
						(*stats).numHit++;
						busRequest = new BusRequest(BusRequest::BusRdX, set, tag,	memoryCost, (*currentJob).getAddress());
					
						jobCycleCost = cacheConstants.getCacheHitCycleCost();
						setLineState(CacheLine::sharedDirty);
						return;
					}
					else if(lineInState(CacheLine::exclusive))
					{
						handleWriteExclusive();
						return;
					}
					else if(lineInState(CacheLine::modified))
					{
						handleWriteModified();
						return;
					}
					else if(lineInState(CacheLine::sharedDirty))
					{
						haveBusRequest = true;
						busy = true;
						unsigned long int  set = 0;
						unsigned long int  tag = 0;
						decode_address((*currentJob).getAddress(), &set, &tag);
						if((tag == 0))
							printf("\n\n\nBIG PROBLEM\n\n\n");
						//its a hit
						unsigned long long memoryCost = cacheConstants.getCacheHitCycleCost();
						(*stats).numHit++;
						busRequest = new BusRequest(BusRequest::BusRdX, set, tag,	memoryCost, (*currentJob).getAddress());
					
						jobCycleCost = cacheConstants.getCacheHitCycleCost();
						setLineState(CacheLine::sharedDirty);
						return;
					}
					else
					{
						haveBusRequest = true;
						busy = true;
						unsigned long int set = 0;
						unsigned long int tag = 0;
						decode_address((*currentJob).getAddress(), &set, &tag);
						if((tag == 0))
							printf("\n\n\nBIG PROBLEM\n\n\n");
						(*stats).numWriteMisses++;
						unsigned long long memoryCost = cacheConstants.getCacheHitCycleCost();
						busRequest = new BusRequest(BusRequest::BusRd, set, tag,	memoryCost, (*currentJob).getAddress());
						
						jobCycleCost = cacheConstants.getCacheHitCycleCost();
						// Need to check shared line to determine what state to start in
						//~ handleWriteSharedInvalid();
						return;
					}
				}
				if(cacheConstants.getProtocol() == CacheConstants::MOESI)
				{
					if(lineInState(CacheLine::invalid) || lineInState(CacheLine::shared))
					{
						handleWriteSharedInvalid();
						return;
					}
					else if(lineInState(CacheLine::exclusive))
					{
						handleWriteExclusive();
						return;
					}
					else if(lineInState(CacheLine::modified))
					{
						handleWriteModified();
						return;
					}
					else if(lineInState(CacheLine::owned))
					{
						haveBusRequest = true;
						busy = true;
						unsigned long int  set = 0;
						unsigned long int  tag = 0;
						decode_address((*currentJob).getAddress(), &set, &tag);
						//its a hit
						if((tag == 0))
							printf("\n\n\nBIG PROBLEM\n\n\n");
						unsigned long long memoryCost = cacheConstants.getCacheHitCycleCost();
						(*stats).numHit++;
						busRequest = new BusRequest(BusRequest::BusRdX, set, tag,	memoryCost, (*currentJob).getAddress());
					
						jobCycleCost = cacheConstants.getCacheHitCycleCost();
						setLineState(CacheLine::modified);
						return;
					}
					else
					{
						handleWriteSharedInvalid();
						return;
					}
				}
			}
			if((*currentJob).isRead())
			{
				(*stats).numReads++;
				if(cacheConstants.getProtocol() == CacheConstants::MSI)
				{
					handleReadRequestMSI();
				}
				if(cacheConstants.getProtocol() == CacheConstants::MESI)
				{
					handleReadRequestMESI();	
								
				}
				if(cacheConstants.getProtocol() == CacheConstants::DRAGON)
				{
					if(lineInState(CacheLine::modified) ||lineInState(CacheLine::exclusive))
					{
						haveBusRequest = false;
						busy = true;
						startServiceCycle = cacheConstants.getCycle();
						jobCycleCost = cacheConstants.getCacheHitCycleCost();
					}
					else if(lineInState(CacheLine::sharedClean) ||lineInState(CacheLine::sharedDirty))
					{	
						haveBusRequest = false;
						busy = true;
						startServiceCycle = cacheConstants.getCycle();
						jobCycleCost = cacheConstants.getCacheHitCycleCost();
						
					}
					else
					{
						haveBusRequest = true;
						busy = true;
						unsigned long int  set = 0;
						unsigned long int  tag = 0;
						decode_address((*currentJob).getAddress(), &set, &tag);
						
						if((tag == 0))
							printf("\n\n\nBIG PROBLEM\n\n\n");
						jobCycleCost = getTotalMemoryCost(set, tag);
						busRequest = new BusRequest(BusRequest::BusRd, set, tag, jobCycleCost, (*currentJob).getAddress());
					    (*stats).numReadMisses++;
						return;
					}
				}
				if(cacheConstants.getProtocol() == CacheConstants::MOESI)
				{
					if(handleReadHit())
					{
						return;
					}
					else
					{
						handleReadMiss();
					}
				}
			}
		}
	}
}









/********************************************************************
 * Function:	hasBusRequest
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description: return True if we have an outstanding bus request
 * 				 to issue, false otherwise
 * 
 * ******************************************************************/
bool Cache::hasBusRequest()
{
	return haveBusRequest;
}











/********************************************************************
 * Function:	getBusRequest
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  return the current busRequest 
 * 
 * ******************************************************************/
BusRequest* Cache::getBusRequest()
{
#ifdef DEBUG
	printf("cache %d got able to put out a bus request for address %llx at cycle %llu \n", 		processorId, (*currentJob).getAddress(), cacheConstants.getCycle());
#endif
	startServiceCycle = cacheConstants.getCycle();
	
	busRequestBeingServiced = true;
	return busRequest;
}








/********************************************************************
 * Function:	handleBusRdShared
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
Cache::SnoopResult Cache::handleBusRdShared(BusRequest* request, unsigned long int  setNum, unsigned long int  tagNum, CacheLine* tempLine)
{
#ifdef DEBUG
	printf("cache number %d just changed set %d and tag %d at address %llx to from shared to shared from a read at cycle %llu \n", 		processorId, setNum, tagNum, (*request).address, cacheConstants.getCycle());
#endif
	return Cache::SHARED;
}





/********************************************************************
 * Function:	handleBusRdModified
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
Cache::SnoopResult Cache::handleBusRdModified(BusRequest* request, unsigned long int  setNum, unsigned long int  tagNum, CacheLine* tempLine)
{
	//FLUSH LINE TO MEMORY and set the state to Shared
	(*stats).numFlush++;
#ifdef DEBUG
	printf("cache %d just FLUSH set %d and tag %d for address %llx \n", processorId, setNum, tagNum, (*request).address);
#endif
	(*tempLine).setState(CacheLine::shared);
	(*stats).numInterventions++;
#ifdef DEBUG
	printf("cache number %d just changed set %d and tag %d for address %llx from modified to shared from a read at cycle %llu \n",		processorId, setNum, tagNum, (*request).address, cacheConstants.getCycle());
#endif
	return 	Cache::FLUSH_MODIFIED_TO_SHARED;
}





/********************************************************************
 * Function:	handleBusRdInvalid
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
Cache::SnoopResult Cache::handleBusRdInvalid(BusRequest* request, unsigned long int  setNum, unsigned long int  tagNum, CacheLine* tempLine)
{
#ifdef DEBUG
	printf("cache number %d does not have set %d and tag %d for address %llx in its cache, ignoring snoop at cycle %llu \n",		processorId, setNum, tagNum, (*request).address, cacheConstants.getCycle());
#endif
	return Cache::NONE;
}







/********************************************************************
 * Function:	handleBusRdXInvalid
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
Cache::SnoopResult Cache::handleBusRdXInvalid(BusRequest* request, unsigned long int  setNum, unsigned long int  tagNum, CacheLine* tempLine)
{
#ifdef DEBUG
	printf("cache number %d does not have set %d and tag %d for address %llx in its cache, ignoring snoop at cycle %llu \n",		processorId, setNum, tagNum, (*request).address, cacheConstants.getCycle());
#endif
	return Cache::NONE;
}






/********************************************************************
 * Function:	handleBusRdXModified
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
Cache::SnoopResult Cache::handleBusRdXModified(BusRequest* request, unsigned long int  setNum, unsigned long int  tagNum, CacheLine* tempLine)
{
	//FLUSH LINE TO MEMORY and set the state to invalid
	(*stats).numFlush++;
#ifdef DEBUG
	printf("cache %d just FLUSH set %d and tag %d \n", processorId, setNum, tagNum);
#endif
	(*tempLine).setState(CacheLine::invalid);

	(*stats).numInvalidations++;
	//~ (*localCache[setNum]).evictLine(tagNum);
#ifdef DEBUG
	printf("cache number %d just changed set %d and tag %d for address %llx from modified to invalid from a ReadX at cycle %llu \n",		processorId, setNum, tagNum, (*request).address, cacheConstants.getCycle());
#endif
	return FLUSH_MODIFIED_TO_INVALID;
}








/********************************************************************
 * Function:	handleBusRdXSharedExclusive
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
Cache::SnoopResult Cache::handleBusRdXSharedExclusive(BusRequest* request, unsigned long int  setNum, unsigned long int  tagNum, CacheLine* tempLine){
	//invalidate ours
	(*tempLine).setState(CacheLine::invalid);
	(*stats).numInvalidations++;
#ifdef DEBUG
	printf("cache number %d just changed set %d and tag %d for address %llx from shared (or exclusive) to invalid from a ReadX at cycle %llu \n",		processorId, setNum, tagNum, (*request).address, cacheConstants.getCycle());
#endif
	return Cache::SHARED;
}







/********************************************************************
 * Function:	handleBusRdMESI
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
Cache::SnoopResult Cache::handleBusRdMESI(BusRequest* request, unsigned long int  setNum, unsigned long int  tagNum, CacheLine* tempLine)
{
	if((*tempLine).getState() == CacheLine::shared)
	{
		return handleBusRdShared(request, setNum, tagNum, tempLine);
	}
	if((*tempLine).getState() == CacheLine::modified)
	{
		return handleBusRdModified(request, setNum, tagNum, tempLine);
	}
	if((*tempLine).getState() == CacheLine::exclusive)
	{
		//so no need to flush, but now we're not exclusive
		(*tempLine).setState(CacheLine::shared);
		(*stats).numInterventions++;
#ifdef DEBUG
		printf("cache number %d just changed set %d and tag %d for address %llx from exclusive to shared from a read at cycle %llu \n",			processorId, setNum, tagNum, (*request).address, cacheConstants.getCycle());
#endif
		return Cache::SHARED;
	}
	else
	{
		return handleBusRdInvalid(request, setNum, tagNum, tempLine);
	}
}








/********************************************************************
 * Function:	handleBusRdXMESI
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
Cache::SnoopResult Cache::handleBusRdXMESI(BusRequest* request, unsigned long int  setNum, unsigned long int  tagNum, CacheLine* tempLine)
{
	if((*tempLine).getState() == CacheLine::shared || (*tempLine).getState() == CacheLine::exclusive)
	{
		return handleBusRdXSharedExclusive(request, setNum, tagNum, tempLine);
	}
	if((*tempLine).getState() == CacheLine::modified)
	{
		return handleBusRdXModified(request, setNum, tagNum, tempLine);
	}
	else
	{
		return handleBusRdXInvalid(request, setNum, tagNum, tempLine);
	}
}






/********************************************************************
 * Function:	handleSnoopMESI
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
Cache::SnoopResult Cache::handleSnoopMESI(BusRequest* request, unsigned long int  setNum, unsigned long int  tagNum, CacheLine* tempLine)
{
	Cache::SnoopResult result = Cache::NONE;
	if((*request).getCommand() == BusRequest::BusRd)
	{
		return handleBusRdMESI(request, setNum, tagNum, tempLine);
	}
	if((*request).getCommand() == BusRequest::BusRdX)
	{
		return handleBusRdXMESI(request, setNum, tagNum, tempLine);
	}
}






/********************************************************************
 * Function:	handleBusRdXMSI
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
Cache::SnoopResult Cache::handleBusRdXMSI(BusRequest* request, unsigned long int  setNum, unsigned long int  tagNum, CacheLine* tempLine)
{
	if((*tempLine).getState() == CacheLine::shared)
	{
		return handleBusRdXSharedExclusive(request, setNum, tagNum, tempLine);
	}
	if((*tempLine).getState() == CacheLine::modified)
	{
		return handleBusRdXModified(request, setNum, tagNum, tempLine);
	}
	else
	{
		return handleBusRdXInvalid(request, setNum, tagNum, tempLine);
	}
}







/********************************************************************
 * Function:	handleBusRdMSI
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
Cache::SnoopResult Cache::handleBusRdMSI(BusRequest* request, unsigned long int  setNum, unsigned long int  tagNum, CacheLine* tempLine)
{
	if((*tempLine).getState() == CacheLine::shared)
	{
		return handleBusRdShared(request, setNum, tagNum, tempLine);
	}
	if((*tempLine).getState() == CacheLine::modified)
	{
		return handleBusRdModified(request, setNum, tagNum, tempLine);
	}
	else
	{
		return handleBusRdInvalid(request, setNum, tagNum, tempLine);
	}
}






/********************************************************************
 * Function:	handleSnoopMSI
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
Cache::SnoopResult Cache::handleSnoopMSI(BusRequest* request, unsigned long int  setNum, unsigned long int  tagNum, CacheLine* tempLine)
{
	if((*request).getCommand() == BusRequest::BusRd)
	{
		return handleBusRdMSI(request, setNum, tagNum, tempLine);
	}
	if ((*request).getCommand() == BusRequest::BusRdX)
	{
		return handleBusRdXMSI(request, setNum, tagNum, tempLine);
	}
}






/********************************************************************
 * Function:	handleSnoopDRAGON
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
Cache::SnoopResult Cache::handleSnoopDRAGON(BusRequest* request, unsigned long int  setNum, unsigned long int  tagNum, CacheLine* tempLine)
{
	Cache::SnoopResult result = Cache::NONE;
	if((*request).getCommand() == BusRequest::BusRd)
	{
		if((*tempLine).getState() == CacheLine::sharedDirty)
		{
			#ifdef DEBUG
			printf("cache number %d just changed set %d and tag %d for address %llx from sharedDirty to sharedClean from a read at cycle %llu \n",			processorId, setNum, tagNum, (*request).address, cacheConstants.getCycle());
			#endif
			//this cache is the owner so we flush block for requesting cache
			(*stats).numFlush++;
			(*request).setSharedLine();
			result = Cache::SHAREDDIRTY;
			//~ (*stats).numWriteBacks++;
			(*tempLine).setState(CacheLine::sharedDirty);
			return result;
		} 
		else if((*tempLine).getState() == CacheLine::modified)
		{
			#ifdef DEBUG
			printf("cache number %d just changed set %d and tag %d for address %llx from modified to sharedDirty from a read at cycle %llu \n",			processorId, setNum, tagNum, (*request).address, cacheConstants.getCycle());
			#endif
			//this cache has the only copy and needs to flush for requesting cache
			//then need to transition to sharedDirty
			(*request).setSharedLine();
			(*stats).numFlush++;
			(*tempLine).setState(CacheLine::sharedDirty);
			(*stats).numInterventions++;
			result = Cache::MODIFIEDTOSHARED;
			return result;
		}
		else if ((*tempLine).getState() == CacheLine::sharedClean)
		{
			#ifdef DEBUG
			printf("cache number %d just changed set %d and tag %d for address %llx from sharedClean to sharedClean from a read at cycle %llu \n",			processorId, setNum, tagNum, (*request).address, cacheConstants.getCycle());
			#endif
			//block supplied by owner if exists or memory
			//if there is a cache that has dirty data will be flushed and at that time we update this block
			(*request).setSharedLine();
			result = Cache::SHAREDCLEAN;
			return result;
		} 
		else if((*tempLine).getState() == CacheLine::exclusive)
		{
			#ifdef DEBUG
			printf("cache number %d just changed set %d and tag %d for address %llx from exclusive to sharedClean from a read at cycle %llu \n",			processorId, setNum, tagNum, (*request).address, cacheConstants.getCycle());
			#endif
			//Block supplied by memory
			//change to shared, share the data
			(*request).setSharedLine();
			result = Cache::EXCLUSIVETOSHARED;
			(*tempLine).setState(CacheLine::sharedClean);
			(*stats).numInterventions++;
			
			return result;
		} 
		else
		{
			#ifdef DEBUG
			printf("cache number %d does not have set %d and tag %d for address %llx in its cache, ignoring snoop at cycle %llu \n",		processorId, setNum, tagNum, (*request).address, cacheConstants.getCycle());
			#endif
			//we don't have the block so do nothing
			return result;
		}
	}
	// This is actually a BusUpd
	if((*request).getCommand() == BusRequest::BusRdX)
	{
		
		if((*tempLine).getState() == CacheLine::sharedDirty)
		{
			#ifdef DEBUG
			printf("cache number %d just changed set %d and tag %d for address %llx from sharedDiryt to sharedClean from a BusUpd at cycle %llu \n",			processorId, setNum, tagNum, (*request).address, cacheConstants.getCycle());
			#endif
			//Another cache wants to write to block so flush the block
			// and transition to shared-modified
			//~ (*stats).numFlush++;
			result = Cache::SHAREDCLEAN;
			(*tempLine).setState(CacheLine::sharedClean);
			return result;
		} 
		else if((*tempLine).getState() == CacheLine::modified)
		{
			#ifdef DEBUG
			printf("cache number %d\n\n just changed set %d and tag %d for address %llx from modified to modified\n\n from a BusUpd at cycle %llu \n",			processorId, setNum, tagNum, (*request).address, cacheConstants.getCycle());
			#endif
			// Can never happen because we should have already 
			// transitioned to shared modified
			return result;
		}
		else if  ((*tempLine).getState() == CacheLine::exclusive)
		{
			#ifdef DEBUG
			printf("cache number %d\n\n just changed set %d and tag %d for address %llx from Exclusive to Exclusive\n\n from a BusUpd at cycle %llu \n",			processorId, setNum, tagNum, (*request).address, cacheConstants.getCycle());
			#endif
			// Can never happen because we should have already 
			// transitioned to shared clean
			return result;
		}
		else if ((*tempLine).getState() == CacheLine::sharedClean)
		{
			#ifdef DEBUG
				printf("cache number %d just changed set %d and tag %d for address %llx from sharedClean to sharedClean from a BusUpd at cycle %llu \n",			processorId, setNum, tagNum, (*request).address, cacheConstants.getCycle());
			#endif
			// We update the value of the block
			(*tempLine).setState(CacheLine::sharedClean);
		} 
		else
		{
			#ifdef DEBUG
				printf("cache number %d does not have set %d and tag %d for address %llx in its cache, ignoring snoop at cycle %llu \n",		processorId, setNum, tagNum, (*request).address, cacheConstants.getCycle());
			#endif
			//so we're invalid, do nothing
		}
	}
	return result;
}









/********************************************************************
 * Function:	handleSnoopMOESI
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
Cache::SnoopResult Cache::handleSnoopMOESI(BusRequest* request, unsigned long int  setNum, unsigned long int  tagNum, CacheLine* tempLine)
{
	Cache::SnoopResult result = Cache::NONE;
	if((*request).getCommand() == BusRequest::BusRd)
	{
		if((*tempLine).getState() == CacheLine::owned)
		{
			//if we have it in owned, we are the one who responds with data
			//no memory use, cacheshare++, miss++
			result = Cache::OWNED;
			(*stats).numCacheShare++;
			return result;
		}
		else if((*tempLine).getState() == CacheLine::modified)
		{
			//if we have it in modified, we are the ones who respond with data, and change line to owned
			//no memory use, cacheshare++, miss++
			(*tempLine).setState(CacheLine::owned);
			(*stats).numCacheShare++;
			result = Cache::MODIFIED;
			return result;
		}
		else if ((*tempLine).getState() == CacheLine::shared)
		{
			//we don't give anything to the req cache cause not our job
			//they have to go to main memory
			result = Cache::SHARED;
			return result;
		} 
		else if((*tempLine).getState() == CacheLine::exclusive)
		{
			//change to shared, share the data
			result = Cache::EXCLUSIVE;
			(*tempLine).setState(CacheLine::shared);
			(*stats).numCacheShare++;
			return result;
		} 
		else
		{
			//so we're invalid, do nothing
			return result;
		}
	}
	if((*request).getCommand() == BusRequest::BusRdX)
	{
		
		if((*tempLine).getState() == CacheLine::owned)
		{
			result = Cache::OWNED;
			(*stats).numCacheShare++;
			(*tempLine).setState(CacheLine::invalid);
			return result;
		} 
		else if((*tempLine).getState() == CacheLine::modified)
		{
			//we only flush when evicted
			result = Cache::MODIFIED; //there is no flush, but can share the data
			(*stats).numCacheShare++;
			
			(*tempLine).setState(CacheLine::invalid);
			return result;
		}
		else if  ((*tempLine).getState() == CacheLine::exclusive)
		{
			result = Cache::EXCLUSIVE;
			(*stats).numCacheShare++;
			(*tempLine).setState(CacheLine::invalid);
			return result;
		}
		else if ((*tempLine).getState() == CacheLine::shared)
		{
			(*tempLine).setState(CacheLine::invalid);
			//don't care about result
		} 
		else
		{
			//so we're invalid, do nothing
		}
	}
	return result;
}











/*

*/
/********************************************************************
 * Function:	snoopBusRequest
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  Read the current BusRequest that another cache issued
 * 				 to the bus and parse it to see if you need to update 
 * 				 our own local cache
 * 
 * ******************************************************************/
Cache::SnoopResult Cache::snoopBusRequest(BusRequest* request)
{

	SnoopResult result = Cache::NONE;
	CacheSet* tempSet = localCache[(*request).getSet()];
	unsigned long int setNum = (*request).getSet();
	unsigned long int tagNum = (*request).getTag();
	
	if((tagNum == 0))
		printf("\n\n\nBIG PROBLEM\n\n\n");
		
	if((*tempSet).hasLine((*request).getTag()))
	{
		//so we do have this line
		CacheLine* tempLine = (*tempSet).getLine((*request).getTag());
		if(cacheConstants.getProtocol() == CacheConstants::MESI)
		{
			return handleSnoopMESI(request, setNum, tagNum, tempLine);
		}
		if(cacheConstants.getProtocol() == CacheConstants::MSI)
		{
			return handleSnoopMSI(request, setNum, tagNum, tempLine);
		}
		if(cacheConstants.getProtocol() == CacheConstants::MOESI)
		{
			return handleSnoopMOESI(request, setNum, tagNum, tempLine);
		}
		if(cacheConstants.getProtocol() == CacheConstants::DRAGON)
		{
			return handleSnoopDRAGON(request, setNum, tagNum, tempLine);
		}
	}
	return result;
}





/********************************************************************
 * Function:	updateEndCycleTime
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
void Cache::updateEndCycleTime(unsigned long long extraCycleCost)
{
	jobCycleCost += extraCycleCost;
}






/********************************************************************
 * Function:	newEndCycleTime
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
void Cache::newEndCycleTime(unsigned long long decrease)
{
	jobCycleCost = decrease;
}










/*

*/
/********************************************************************
 * Function:	busJobDone
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  Update to store the new line requested
 * 
 * ******************************************************************/
void Cache::busJobDone(bool isShared)
{
	unsigned long long jobAddr = (*currentJob).getAddress();
	unsigned long int  currJobSet = 0;
	unsigned long int  currJobTag = 0;
	
	decode_address(jobAddr, &currJobSet, &currJobTag);
	if((currJobTag == 0))
		printf("\n\n\nBIG PROBLEM\n\n\n");
	
	haveBusRequest = false;
	busy = false;
	busRequestBeingServiced = false;
	CacheSet* currSet = localCache[currJobSet];
		
	//~ if((cacheConstants.getProtocol() == CacheConstants::MESI)||(cacheConstants.getProtocol() == CacheConstants::MSI))
	//~ {
		//~ if((*currSet).allLinesFull())
		//~ {
			//~ if((*currSet).hasInvalidLine())
				//~ (*currSet).evictLRULine();//(*currSet).evictLRULineInvalid();
			//~ else if (!((*currSet).hasLine(currJobTag)))
			//~ {
//~ 
				//~ if((*currSet).evictLineModified())
				//~ {
					//~ (*currSet).evictLRULine();
					//~ (*stats).numWriteBacks++;	
				//~ }
				//~ else if((*currSet).evictLineShared())
				//~ {
					//~ (*currSet).evictLRULine();
//~ 
				//~ }
				//~ else
				//~ {
					printf("should never happen\n");
					//~ (*currSet).evictLRULine();
				//~ }
			//~ }
		//~ }
	//~ }
	//~ else
	{
		//Need to tell if we need to evict a line from the set  no Null lines and no 
		bool needToEvict = (*currSet).isFull() && !((*currSet).hasLine(currJobTag));
		if (needToEvict)
		{
			(*stats).numWriteBacks++;	
			(*currSet).evictLRULine();
			(*stats).numEvict++;
		}
	}
	
	if (!(*currSet).hasLine(currJobTag))
	{
		CacheLine* newLine = new CacheLine(jobAddr, currJobSet, currJobTag);
	
		//~ (*currSet).addLine(newLine);
		(*currSet).replaceLine(newLine);
		if(cacheConstants.getProtocol() != CacheConstants::DRAGON)
		{
			if((*currentJob).isWrite())
				(*stats).numWriteMisses++;
			//~ else
				//~ (*stats).numReadMisses++;	
		}
	}
		
		
	CacheLine* currLine = (*currSet).getLine(currJobTag); 
	(*currLine).lastUsedCycle = cacheConstants.getCycle();
	
	if((*currentJob).isWrite())
	{
		if(isShared && cacheConstants.getProtocol() == CacheConstants::MOESI)
		{
			//we already calculated if it was a share or not
			(*currLine).setState(CacheLine::modified);
			//share in this case means that we got the data from a modified or an owned cache or an exclusive
		}
		if (cacheConstants.getProtocol() == CacheConstants::MOESI && (*currLine).getState() != CacheLine::owned)
		{
			//so it wasn't shared
			//~ printf("~~~ write was from mem \n");
			(*stats).numMainMemoryUses++;
			(*currLine).setState(CacheLine::modified);
		}
		if(isShared && cacheConstants.getProtocol() == CacheConstants::DRAGON)
		{
			// we need to issue a new busrequest for busupdate/busreadx
			// we first issued BusRd
			haveBusRequest = true;
			busy = true;
			jobCycleCost = cacheConstants.getCacheResponseCycleCost();
			busRequest = new BusRequest(BusRequest::BusRdX, currJobSet, currJobTag,
				jobCycleCost, jobAddr);
		
			(*currLine).setState(CacheLine::sharedDirty);
			//share in this case means that we got the data from a modified or an owned cache or an exclusive
		}
		if (!isShared && cacheConstants.getProtocol() == CacheConstants::DRAGON)
		{
			if((*currLine).getState() == CacheLine::invalid)
					(*currLine).setState(CacheLine::modified);
			(*stats).numMainMemoryUses++;

		}
		if(isShared && cacheConstants.getProtocol() == CacheConstants::MESI)
		{
			(*stats).numCacheShare++;
			//~ printf("~~~~~~~ share++ \n");
			(*currLine).setState(CacheLine::modified);
		//~ printf("cache %d has just been told it has finished a job for address %llx and stored in modified state at cycle %llu \n",
			//~ processorId, (*currentJob).getAddress(), cacheConstants.getCycle());
		}
		if( cacheConstants.getProtocol() == CacheConstants::MSI)
		{
			//msi protocol, we had to read from memory
			(*stats).numMainMemoryUses++;
			//~ printf("~~~~~~~~~~~~ mem use ++ \n");
			(*currLine).setState(CacheLine::modified);
		//~ printf("cache %d has just been told it has finished a job for address %llx and stored in modified state at cycle %llu \n",
			//~ processorId, (*currentJob).getAddress(), cacheConstants.getCycle());
		}
		else
		{
		}
		//~ (*currLine).setState(CacheLine::modified);
		//~ printf("cache %d has just been told it has finished a job for address %llx and stored in modified state at cycle %llu \n",
			//~ processorId, (*currentJob).getAddress(), cacheConstants.getCycle());
	}
	
	if((*currentJob).isRead())
	{
		if(cacheConstants.getProtocol() == CacheConstants::MOESI)
		{
			if(isShared)
			{
				//so someone was modified or owned or exclusive
				//~ printf("just got my read request as a share \n");
				(*currLine).setState(CacheLine::shared);
			}
			else
			{
				//so we exclusive, and had to get from main memory
				//~ printf("my read req was exclusive ~~~~~~~~~~ \n");
				(*currLine).setState(CacheLine::exclusive);
				(*stats).numMainMemoryUses++;
			}
		}
		if(cacheConstants.getProtocol() == CacheConstants::DRAGON)
		{
			if(isShared)
			{
				//so someone was modified or owned or exclusive
				//~ printf("just got my read request as a share \n");
				(*currLine).setState(CacheLine::sharedClean);
			}
			else
			{
				//so we exclusive, and had to get from main memory
				//~ printf("my read req was exclusive ~~~~~~~~~~ \n");
				(*currLine).setState(CacheLine::exclusive);
				(*stats).numMainMemoryUses++;
			}
			
		}
		if((cacheConstants).getProtocol() == CacheConstants::MESI)
		{
			if(!isShared)
			{
				(*currLine).setState(CacheLine::exclusive);
				(*stats).numMainMemoryUses++;
				//~ printf("main mem ++");
				//~ printf("cache %d has just been told it has finished a job for address %llx and stored in exclusive state at cycle %llu \n", 
					//~ processorId, (*currentJob).getAddress(), cacheConstants.getCycle());
			}
			else
			{
				(*currLine).setState(CacheLine::shared);
				(*stats).numCacheShare++;
				//~ printf("share ++ \n");
				//~ printf("cache %d has just been told it has finished a job for address %llx and stored in shared state at cycle %llu \n", 
					//~ processorId, (*currentJob).getAddress(), cacheConstants.getCycle());
			}
		}
		if(cacheConstants.getProtocol() == CacheConstants::MSI)
		{	
			(*stats).numMainMemoryUses++;
			//~ printf("mem++ ~~~~~~~~~~~~~~~ \n");
			(*currLine).setState(CacheLine::shared);
	
			//~ printf("cache %d has just been told it has finished a job for address %llx and stored in shared state at cycle %llu \n", 
				//~ processorId, (*currentJob).getAddress(), cacheConstants.getCycle());
		}
	}
	if((int)currLine->getState() == 0)
		printf("State: %i\n",(int)currLine->getState());
}











/********************************************************************
 * Function:	getProcessorId
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
int Cache::getProcessorId()
{
	return processorId;
}






/********************************************************************
 * Function:	updateCurrentJobLineCycle
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
void Cache::updateCurrentJobLineCycle()
{
	unsigned long int  set = 0;
	unsigned long int  tag = 0;
	if(currentJob == NULL)
	{
		return;
	}
	busy = false;
	decode_address((*currentJob).getAddress(), &set, &tag);
	//~ ShowLine(set);
	if((tag == 0))
		printf("\n\n\nBIG PROBLEM\n\n\n");
		
	if((localCache[set] != NULL) && (*localCache[set]).hasLine(tag))
	{
		CacheLine* theLine = (*localCache[set]).getLine(tag);
		(*theLine).lastUsedCycle = cacheConstants.getCycle();
	}
	
}







/********************************************************************
 * Function:	tick
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
void Cache::tick()
{

	if(busRequestBeingServiced)
	{
		return; 
		//we have to wait for the bus to tell us we're done, not our own selves saying it
		//since cache time incremented before bus time
	}

	if(startServiceCycle + jobCycleCost <= cacheConstants.getCycle())
	{
		//finished a job
		updateCurrentJobLineCycle();
		busy = false;
	}

	if(!busy && pendingJobs.size() != 0)
	{
		//so we're free to do a new request
		handleRequest();
	}
}






/********************************************************************
 * Function:	printStats
 * Inputs:		Cache printing stats for
 * Outputs:		None
 * Description:  print out the rest of 
 * 			statistics here, following the 
 * 			ouput file format.
 * 
 * ******************************************************************/
void Cache::printStats()
{ 
	unsigned long long numMemTransactions;
	
	(*stats).missRate = (float)((*stats).numReadMisses +(*stats).numWriteMisses)/((*stats).numReads+(*stats).numWrites)*100;
	
	//readmiss+writemiss+writeback+somemore 	somemore = write on a Shared block
	if(cacheConstants.getProtocol() == CacheConstants::MSI)
		numMemTransactions = ((*stats).numWriteMisses + (*stats).numReadMisses + (*stats).numWriteBacks + (*stats).numWriteShared);
	// readmiss+writemiss+writeback-cache2cache
	else if(cacheConstants.getProtocol() == CacheConstants::MESI)	
		numMemTransactions = ((*stats).numWriteMisses + (*stats).numReadMisses + (*stats).numWriteBacks - (*stats).numCacheShare );
	// readmiss+writemiss+writeback
	else if(cacheConstants.getProtocol() == CacheConstants::DRAGON)
		numMemTransactions = ((*stats).numWriteMisses + (*stats).numReadMisses + (*stats).numWriteBacks);

	/****print out the rest of statistics here.****/
	/****follow the ouput file format**************/
	printf("===== Simulation results (Cache %d)     =====\n",processorId);
	printf("01. number of reads:	:			%lld\n",(*stats).numReads);
	printf("02. number of read misses:			%lld\n",(*stats).numReadMisses);
	printf("03. number of writes:				%lld\n",(*stats).numWrites);
	printf("04. number of write misses:			%lld\n",(*stats).numWriteMisses);
	printf("05. total miss rate:				%.2f%%\n",(*stats).missRate);
	printf("06. number of writebacks:			%lld\n", (*stats).numWriteBacks);
	printf("07. number of cache-to-cache transfers:		%lld\n", (*stats).numCacheShare);	
	printf("08. number of memory transactions:		%lld\n",numMemTransactions);
	printf("09. number of interventions:			%lld\n",(*stats).numInterventions);
	printf("10. number of invalidations:			%lld\n",(*stats).numInvalidations);
	printf("11. number of flushes:				%lld\n",(*stats).numFlush);
	printf("12. number of BusRdX:				%lld\n",(*stats).numBusReadX);
	

#ifdef DEBUG
	/****print out the rest of statistics here.****/
	/****follow the ouput file format**************/
	printf("===== Simulation results (Cache %d)     =====\n",processorId);
	printf("%lld\n",(*stats).numReads);
	printf("%lld\n",(*stats).numReadMisses);
	printf("%lld\n",(*stats).numWrites);
	printf("%lld\n",(*stats).numWriteMisses);
	printf("%.2f%%\n",(*stats).missRate);
	printf("%lld\n", (*stats).numWriteBacks);
	printf("%lld\n", (*stats).numCacheShare);	
	printf("%lld\n",numMemTransactions);
	printf("%lld\n",(*stats).numInterventions);
	printf("%lld\n",(*stats).numInvalidations);
	printf("%lld\n",(*stats).numFlush);
	printf("%lld\n",(*stats).numBusReadX);
	
	printf("07. number of hits:						%lld\n", (*stats).numHit);	
	printf("08. number of miss:						%lld\n",(*stats).numMiss);
	printf("09. number of Excl/Mod to Shared:		%lld\n",(*stats).numExclusiveToModifiedTransitions);
	printf("10. number of Bus Requests:				%lld\n",(*stats).numBusRequests);
	printf("11. number of Main Memory Uses:				%lld\n",(*stats).numMainMemoryUses);
	printf("12. number of BusRdX:				%lld\n",(*stats).numBusReadX);
	printf("12. number of Evictions:				%lld\n",(*stats).numEvict);
#endif
	
}


Cache::~Cache(void)
{

}
