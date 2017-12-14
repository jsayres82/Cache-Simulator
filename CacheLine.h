#ifndef CACHELINE_H
#define CACHELINE_H
#include "CacheConstants.h"
#include "stdbool.h"





class CacheLine
{
public:
	typedef enum {invalid, shared, modified, exclusive, owned, sharedClean, sharedDirty} State;
	unsigned long long rawAddress; //raw address that maps to me (ignoring block offset)
	long unsigned int setIndex; //what set i'm in
	long unsigned int myTag; //unique tag identifier
	bool lineShared;
	CacheLine::State myState;
	unsigned long long lastUsedCycle;
	CacheLine(unsigned long long, long unsigned int, long unsigned int);
	unsigned long long getAddress();
	long unsigned int getTag();
	long unsigned int getSetIndex();
	State getState();
	bool isShared();
	void setState(State state);
	bool getLineValid();
	~CacheLine(void);
};

#endif
