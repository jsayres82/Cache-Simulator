#include "CacheConstants.h"
#include "string"
#include <cmath>
#include <stdio.h>


int cacheHitCycleCost;
int memoryResponseCycleCost;
int cacheResponseCycleCost;
int numProcessors;

unsigned long int numSets;
unsigned long int  numSetBits;
unsigned long int  numLinesInSet;
unsigned long int  numBytesInLine;
unsigned long int  numBytesBits;
unsigned long int  numAddressBits; //how many bits are the addresses
unsigned long int  numCacheSize; //= 2^sets * 2^lines * bytesPerLine
unsigned long long cycles; //total count of all cycles elapsed in simulation
CacheConstants::Protocol protocol; //string representing what the protocol is 






/********************************************************************
 * Function:	CacheConstants
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
CacheConstants::CacheConstants(void)
{
	cacheResponseCycleCost = CacheConstants::getCacheResponseCycleCost();
}







/********************************************************************
 * Function:	CacheConstants
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
CacheConstants::CacheConstants(int size, int assoc, int lineSize, int numProc, int pro )
{
	int tempVal, i;

	if(pro == 0)
	{
		protocol = CacheConstants::MSI;
	}
	else if(pro == 1)
	{
		protocol = CacheConstants::MESI;
	}
	else if(pro == 2)
	{
		protocol = CacheConstants::DRAGON;
	}
	else
	{
		protocol = CacheConstants::MOESI;
	}

	

	cacheHitCycleCost = 2;
	memoryResponseCycleCost = 90;
	cacheResponseCycleCost = 60; 
	numProcessors = numProc; //4 core, hyperthreading
	

	numLinesInSet = assoc; //Number of Blocks in Set associativity
	numBytesInLine = lineSize; //Block Size
	
	numBytesBits = (int)log2(numBytesInLine); //Block Offset Bits
	printf("Byte Bits: %i\n", numBytesBits);
	numCacheSize = size;// (numSets * numLinesInSet * numBytesInLine); //~32k
	numAddressBits = 32;
	cycles = 0;
	tagMask = 0;

	
	tempVal = numLinesInSet * numBytesInLine;
	printf("TempVal: %i\n", tempVal);
	numSets = size/tempVal;  //totalCacheSize / (numLinesInSet * (numBytesInLine));
	printf("Number Sets: %i\n", numSets);
	numSetBits = (int)log2(numSets); ////Index Number of bits in set
	printf("numSetBits %i\n", numSetBits);
	tagMask =0;
	
    for(i=0;i<(numSetBits);i++)
    {
 		tagMask <<= 1;
 		tagMask |= 1;
 		printf("tagMask: %x\n", tagMask);
    }
   
    printf("tagMask: %i\n", tagMask);
}






/********************************************************************
 * Function:	setProtocol
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
void CacheConstants::setProtocol(int p)
{
	protocol = (Protocol)p;	
}






/********************************************************************
 * Function:	setNumProcessors
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
void CacheConstants::setNumProcessors(int numProc)
{
	numProcessors = numProc;
}








/********************************************************************
 * Function:	setNumLinesInSet
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
void CacheConstants::setNumLinesInSet(int assoc)
{
	numLinesInSet = assoc;
	CacheConstants::setNumSets();
	CacheConstants::setNumBytesBits();
	CacheConstants::setNumSetBits();
}







/********************************************************************
 * Function:	setNumBytesInLine
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
void CacheConstants::setNumBytesInLine(int blockSize)
{
	numBytesInLine = blockSize;
	CacheConstants::setNumSets();
	CacheConstants::setNumBytesBits();
	CacheConstants::setNumSetBits();
}
	
	
	
	
	


/********************************************************************
 * Function:	setNumCacheSize
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
void CacheConstants::setNumCacheSize(int cacheSize)
{
	numCacheSize = cacheSize;
	CacheConstants::setNumSets();
	CacheConstants::setNumBytesBits();
	CacheConstants::setNumSetBits();

}







/********************************************************************
 * Function:	setNumAddressBits
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
void setNumAddressBits(int addrBits)
{
	numAddressBits = addrBits;
}






/********************************************************************
 * Function:	getCacheHitCycleCost
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
void getCacheHitCycleCost(int cacheHitCost)
{
	cacheHitCycleCost = cacheHitCost;
}








/********************************************************************
 * Function:	setCacheResponseCycleCost
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
void setCacheResponseCycleCost(int cacheRespCost)
{
	cacheResponseCycleCost = cacheRespCost;
}




/********************************************************************
 * Function:	setMemoryResponseCycleCost
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
void setMemoryResponseCycleCost(int memRespCost)
{
	memoryResponseCycleCost = memRespCost;
}







/********************************************************************
 * Function:	getTagMask
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
int CacheConstants::getTagMask()
{
	return tagMask;
}






/********************************************************************
 * Function:	getCacheResponseCycleCost
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
int CacheConstants::getCacheResponseCycleCost()
{
	return cacheResponseCycleCost;
}







/********************************************************************
 * Function:	getNumAddressBits
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
int CacheConstants::getNumAddressBits()
{
	return numAddressBits;
}
	
	
	

	
	
	
	
	
/********************************************************************
 * Function:	getProtocol
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
CacheConstants::Protocol CacheConstants::getProtocol()
{
	return protocol;
}







/********************************************************************
 * Function:	getNumSetBits
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
int CacheConstants::getNumSetBits()
{
	return numSetBits;
}






/********************************************************************
 * Function:	getNumBytesBits
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
int CacheConstants::getNumBytesBits()
{
	return numBytesBits;
}







/********************************************************************
 * Function:	getCacheHitCycleCost
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
int CacheConstants::getCacheHitCycleCost()
{
	return cacheHitCycleCost;
}







/********************************************************************
 * Function:	getMemoryResponseCycleCost
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
int CacheConstants::getMemoryResponseCycleCost()
{
	return memoryResponseCycleCost;
}








/********************************************************************
 * Function:	getNumProcessors
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
int CacheConstants::getNumProcessors()
{
	return numProcessors;
}







/********************************************************************
 * Function:	getNumSets
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
int CacheConstants::getNumSets()
{
	return numSets;
}








/********************************************************************
 * Function:	getNumLinesInSet
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
int CacheConstants::getNumLinesInSet()
{
	return numLinesInSet;
}







/********************************************************************
 * Function:	getNumBytesInLine
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
int CacheConstants::getNumBytesInLine()
{
	return numBytesInLine;
}







/********************************************************************
 * Function:	getNumCacheSize
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
int CacheConstants::getNumCacheSize()
{
	return numCacheSize;
}







/********************************************************************
 * Function:	getCycle
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
unsigned long long CacheConstants::getCycle()
{
	return cycles;
}







/********************************************************************
 * Function:	tick
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
void CacheConstants::tick()
{
	cycles++;
}





/********************************************************************
 * Function:	setNumSets
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
void CacheConstants::setNumSets()
{
	numSets = ((numCacheSize/ numBytesInLine) / numLinesInSet);
}






/********************************************************************
 * Function:	setNumBytesBits
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
void CacheConstants::setNumBytesBits()
{
	numBytesBits = log2(numBytesBits);
}





/********************************************************************
 * Function:	setNumSetBits
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
void CacheConstants::setNumSetBits()
{
	numSetBits = log2(numSets);
}



/********************************************************************
 * Function:	CacheConstants
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
CacheConstants::~CacheConstants(void)
{
}
