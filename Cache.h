#ifndef CACHE_H
#define CACHE_H
#include "CacheConstants.h"
#include "CacheJob.h"
#include "queue"
#include "BusRequest.h"
#include "CacheConstants.h"
#include "CacheController.h"	
#include "CacheSet.h"
#include "vector"
#include "CacheJob.h"
#include "queue"
#include "BusRequest.h"
#include "CacheLine.h"
#include "CacheStats.h"





class Cache
{
public:

	CacheConstants cacheConstants;
	std::vector<CacheSet*> localCache;
	std::queue<CacheJob*> pendingJobs;
	CacheJob* currentJob;
	BusRequest* busRequest;
	CacheStats* stats;
	int processorId;
	bool haveBusRequest;
	bool busy;
	bool busRequestBeingServiced;
	unsigned long long startServiceCycle;
	unsigned long long jobCycleCost;
	CacheLine::State currentBusJobResultState;

	void updateEndCycleTime(unsigned long long);
	typedef enum {SHARED, FLUSH_MODIFIED_TO_SHARED,FLUSH_MODIFIED_TO_INVALID, EXCLUSIVE, OWNED, MODIFIED, NONE, SHAREDCLEAN, SHAREDDIRTY, EXCLUSIVETOSHARED, MODIFIEDTOSHARED} SnoopResult;	 
	Cache(int, CacheConstants,std::queue<CacheJob*>*, CacheStats*);
	int getProcessorId();
	void setPId(int);
	void handleRequest();
	void tick();
	void busJobDone(bool);
	bool hasBusRequest();
	void decode_address(unsigned long long address, unsigned long int * whichSet, unsigned long int * tag);
	unsigned long long getTotalMemoryCost(unsigned long int  set, unsigned long int  tag);
	bool lineInState(CacheLine::State state);
	void setLineState(CacheLine::State state);
	void updateCurrentJobLineCycle();
	BusRequest* getBusRequest();
	Cache::SnoopResult snoopBusRequest(BusRequest*);
	void newEndCycleTime(unsigned long long);
	void printStats();
	void ShowCache();
	void ShowLine(unsigned long int line);
	bool isLineValid();
	~Cache(void);
private:
	void handleWriteRequestMESI();
	void handleReadRequestMESI();
	void handleWriteRequestMSI();
	void handleReadRequestMSI();
	bool handleWriteModified();
	bool handleWriteExclusive();
	void handleWriteSharedInvalid();
	bool handleReadHit();
	void handleReadMiss();
	Cache::SnoopResult handleSnoopMESI(BusRequest*, unsigned long int , unsigned long int , CacheLine*);
	Cache::SnoopResult handleSnoopMSI(BusRequest*, unsigned long int , unsigned long int , CacheLine*);
	Cache::SnoopResult handleSnoopDRAGON(BusRequest*, unsigned long int , unsigned long int , CacheLine*);
	Cache::SnoopResult handleSnoopMOESI(BusRequest*, unsigned long int , unsigned long int , CacheLine*);
	Cache::SnoopResult handleBusRdShared(BusRequest*, unsigned long int , unsigned long int , CacheLine*);
	Cache::SnoopResult handleBusRdModified(BusRequest*, unsigned long int , unsigned long int , CacheLine*);
	Cache::SnoopResult handleBusRdInvalid(BusRequest*, unsigned long int , unsigned long int , CacheLine*);
	Cache::SnoopResult handleBusRdXInvalid(BusRequest*, unsigned long int , unsigned long int , CacheLine*);
	Cache::SnoopResult handleBusRdXModified(BusRequest*, unsigned long int , unsigned long int , CacheLine*);
	Cache::SnoopResult handleBusRdXSharedExclusive(BusRequest*, unsigned long int , unsigned long int , CacheLine*);
	Cache::SnoopResult handleBusRdXMESI(BusRequest*, unsigned long int , unsigned long int , CacheLine*);
	Cache::SnoopResult handleBusRdMESI(BusRequest*, unsigned long int , unsigned long int , CacheLine*);
	Cache::SnoopResult handleBusRdXMSI(BusRequest*, unsigned long int , unsigned long int , CacheLine*);
	Cache::SnoopResult handleBusRdMSI(BusRequest*, unsigned long int , unsigned long int , CacheLine*);


};

#endif
