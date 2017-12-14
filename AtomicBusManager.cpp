#include "AtomicBusManager.h"
#include "CacheConstants.h"
#include "Cache.h"
#include "vector"
#include "CacheJob.h"
#include "BusRequest.h"





/********************************************************************
 * Function:	AtomicBusManager
 * Inputs:		None
 * Outputs:		None
 * Description:  Container of the lines for an individual set.
 * 				Manages requests for lines and LRU eviction
 * 
 * ******************************************************************/
AtomicBusManager::AtomicBusManager(CacheConstants consts, std::vector<Cache*>* allCaches)
{
	constants = consts;
	caches = *allCaches;
	currentCache = 0;
	startCycle = 0;
	endCycle = 0;
	inUse = false;
	isShared = false;
}







/********************************************************************
 * Function:	tick
 * Inputs:		None
 * Outputs:		None
 * Description:  Handle checking if the current BusRequest is completed
 * 				And if so, getting a new one and broadcasting it to all
 * 				other caches
 * 
 * ******************************************************************/
void AtomicBusManager::tick()
{

	if(inUse)
	{
		//so the current job being executed is completed this cycle 
		if(endCycle <= constants.getCycle())
		{
			currentRequest = (*caches.at(currentCache)).getBusRequest();
						
			if(constants.getProtocol() == CacheConstants::MSI)
			{
				if(currentRequest->getCommand() == BusRequest::BusRdX)
					(*caches[currentCache]).stats->numBusReadX++;
			}
			
			//tell the cache that its job is done
			(*caches.at(currentCache)).busJobDone(isShared);
			currentCache = -1;

			inUse = false;
			isShared = false;
		}
		else
		{
			return;
		}
	}

	int tempNextCache = -1;
	//so either not in use, or we just finished a job
	//TODO: actually make this round robin
	for(int i = 0; i < constants.getNumProcessors(); i++)
	{
		if(((caches.at(i)) != NULL) && (*caches.at(i)).hasBusRequest())
		{
			//so we will now service this cache
			currentRequest = (*caches.at(i)).getBusRequest();
			tempNextCache = i;
			break;
		}
	}

	if(tempNextCache == -1)
	{
		//so there are no more pending requests in the system
		//printf("no one to service, leaving \n");
		inUse = false;
		isShared = false;
		return;
	}
	
	
	currentCache = tempNextCache;
	//~ printf("now servicing cache %d on the bus at cycle %llu \n", currentCache, constants.getCycle());
	(*caches[currentCache]).stats->numBusRequests++;
	
	//since only get here if we got a new job
	//update the startCycle for when we just changed jobs
	startCycle = constants.getCycle();
	endCycle = startCycle + (*currentRequest).getCycleCost(); 
	inUse = true;

	bool foundShared = false;
	//so now we have the new currentRequest and currentCache is the cache that asked for that request
	//so now we broadcast this currentRequest to all the caches other than the one who sent it
	for(int i = 0; i < constants.getNumProcessors(); i++)
	{
		if(i != currentCache)
		{
			Cache::SnoopResult result = (*caches.at(i)).snoopBusRequest(currentRequest);
			
			if(constants.getProtocol() == CacheConstants::MSI)
			{
	
				if (result == Cache::FLUSH_MODIFIED_TO_SHARED || result == Cache::FLUSH_MODIFIED_TO_INVALID)
				{
					(*caches[i]).stats->numWriteBacks++;
					//flush to memory, then load from memory
					endCycle += constants.getMemoryResponseCycleCost();
					(*caches[currentCache]).updateEndCycleTime(constants.getMemoryResponseCycleCost());
					//make sure the cache itself knows that it isn't finished until the proper time
					(*caches[currentCache]).stats->numMainMemoryUses++;
					//~ printf("num mem use ++ \n");
					continue;
				}
				if (result == Cache::SHARED)
				{
					//do nothing, no sharing in MSI
					continue;
				}
				if (result == Cache::NONE)
				{
					//Do nothing
					continue;
				}
			}			
			if(constants.getProtocol() == CacheConstants::MESI)
			{
				if(result == Cache::FLUSH_MODIFIED_TO_INVALID)
				{
					(*caches[i]).stats->numWriteBacks++;
					endCycle += constants.getCacheResponseCycleCost();
					(*caches[currentCache]).updateEndCycleTime(constants.getCacheResponseCycleCost());
					isShared = true;
					//so requesting cache should set the line to modified
					//this happens from a busrdx command
					(*caches[currentCache]).stats->numBusReadX++;
					(*caches[currentCache]).stats->numMainMemoryUses++;
					//~ printf("num main mem use ++ \n");
					continue;
				}
				if(result == Cache::FLUSH_MODIFIED_TO_SHARED)
				{
					//so this happens when there is a busrd req
					endCycle += constants.getCacheResponseCycleCost();
					(*caches[currentCache]).updateEndCycleTime(constants.getCacheResponseCycleCost());
					//so tell the cache that it shoudl set the line to shared
					isShared = true;
					(*caches[currentCache]).stats->numMainMemoryUses++;
					//~ printf("num main mem use ++ \n");
					continue;
				}
				if(result == Cache::SHARED)
				{
					if(!foundShared)
					{
						endCycle -= (constants.getMemoryResponseCycleCost() - constants.getCacheResponseCycleCost());
						(*caches[currentCache]).newEndCycleTime(constants.getCacheResponseCycleCost());
						foundShared = true;
					}
					isShared = true;
					continue;
				}
				if(result == Cache::NONE)
				{
					//nothing
					continue;
				}
			}
			if(constants.getProtocol() == CacheConstants::DRAGON)
			{
				if(currentRequest->isLineShared() == true)
				{
					isShared = true;
				}
				else
				{
					isShared = false;
				}
			}
			
			if(constants.getProtocol() == CacheConstants::MOESI)
			{
				if(result == Cache::MODIFIED || result == Cache::EXCLUSIVE || result == Cache::OWNED)
				{
					//so adjust the cycle cost to a share
					//but don't adjust cost if it was an upgrade from owned to modified, cause that's bad
					if((*currentRequest).getCycleCost() == constants.getCacheHitCycleCost())
					{
						//so was a bus upgrade essentially
						isShared = true;
						continue;
					}
					else
					{
						endCycle -= (constants.getMemoryResponseCycleCost() - constants.getCacheResponseCycleCost());
						(*caches[currentCache]).newEndCycleTime(constants.getCacheResponseCycleCost());
						isShared = true;
					}
				}
			}
		}
		//so now all caches have acknowledged the new BusRequest that was issued
	}
	if(!foundShared && (constants.getProtocol() == CacheConstants::MESI))
	{
		//so if we never did get a shared, had to get from main memoryu
		(*caches[currentCache]).stats->numMainMemoryUses++;
	}
}




/********************************************************************
 * Function:	AtomicBusManager
 * Inputs:		None
 * Outputs:		None
 * Description:  Container of the lines for an individual set.
 * 				Manages requests for lines and LRU eviction
 * 
 * ******************************************************************/
AtomicBusManager::~AtomicBusManager(void){
}
