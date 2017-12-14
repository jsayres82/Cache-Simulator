
#ifndef CACHECONSTANTS_H
#define CACHECONSTANTS_H
#include "string"





//~ #define DEBUG



class CacheConstants
{
public:
	CacheConstants(void);
	CacheConstants(int size, int lineSize, int assoc, int protocol, int numProc);
	typedef enum {MSI=0, MESI, DRAGON, MOESI} Protocol;
	int getCacheHitCycleCost();
	int getMemoryResponseCycleCost();
	int getCacheResponseCycleCost();
	int getNumProcessors();
	int getNumSets();
	int getNumLinesInSet();
	int getNumBytesInLine();
	int getNumCacheSize();
	int getTagMask();
	unsigned long long getCycle();
	void tick();
	Protocol getProtocol();
	int getNumBytesBits();
	int getNumSetBits();
	int getNumAddressBits();
	int tagMask;
	

	void setCacheHitCycleCost(int);
	void setCacheResponseCycleCost(int);
	void setMemoryResponseCycleCost(int);
	void setNumBytesBits();
	void setNumSetBits();
	void setNumAddressBits(int);
	void setNumSets();
	
	void setProtocol(int);
	void setNumProcessors(int);
	void setNumLinesInSet(int);
	void setNumBytesInLine(int);
	void setNumCacheSize(int);

	~CacheConstants(void);
};

#endif
