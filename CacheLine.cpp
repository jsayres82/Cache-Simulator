#include "CacheLine.h"
#include "CacheConstants.h"




/********************************************************************
 * Function:	CacheLine
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
CacheLine::CacheLine(unsigned long long address, long unsigned int set, long unsigned int tag)
{
	rawAddress = address;
	setIndex = set;
	myTag = tag;
	myState = invalid;//invalid;
	lastUsedCycle = 0;//CacheConstants::getCycle();
	lineShared = false;
}

/********************************************************************
 * Function:	isValid
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
bool CacheLine::getLineValid()
{
	return (myState != CacheLine::invalid);
}



/********************************************************************
 * Function:	getState
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
CacheLine::State CacheLine::getState()
{
	return myState;
}




/********************************************************************
 * Function:	isShared
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
bool CacheLine::isShared()
{
	return lineShared;
}




/********************************************************************
 * Function:	setState
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
void CacheLine::setState(State state)
{
	myState = state;
}




/********************************************************************
 * Function:	getAddress
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
unsigned long long CacheLine::getAddress()
{
	return rawAddress;
}





/********************************************************************
 * Function:	getSetIndex
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
long unsigned int CacheLine::getSetIndex()
{
	return setIndex;
}






/********************************************************************
 * Function:	getTag
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
long unsigned int CacheLine::getTag()
{
	return myTag;
}





/********************************************************************
 * Function:	CacheLine
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
CacheLine::~CacheLine(void)
{
}
