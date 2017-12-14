#ifndef	CACHESET_H
#define CACHESET_H
#include "CacheConstants.h"
#include "CacheLine.h"
#include "vector"






class CacheSet
{
public:
	long unsigned int  setID;
	std::vector<CacheLine*> allLines;
	CacheConstants* consts;
	CacheSet(CacheConstants* , long unsigned int);
	bool hasLine(long unsigned int);
	CacheLine* getLine(long unsigned int);
	~CacheSet(void);
	bool isFull();
	void evictLRULine();
	void evictLRULineInvalid();
	bool evictLineModified();
	bool evictLineInvalid();
	bool evictLineShared();
	void addLine(CacheLine*);
	bool allLinesFull();
	bool hasInvalidLine();
	void evictLine(long unsigned int tag);
	void getLRU();
	bool lineValid(long unsigned int tag);
	CacheLine* getLRULine();
	void replaceLine(CacheLine* line);
	
};

#endif
