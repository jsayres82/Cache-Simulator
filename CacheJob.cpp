#include "CacheJob.h"




/********************************************************************
 * Function:	CacheJob Constructor
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
CacheJob::CacheJob()
{
}






/********************************************************************
 * Function:	CacheJob Constructor
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
CacheJob::CacheJob(char readWrite, unsigned long long addr, int tId)
{
	rW = readWrite;
	address = addr;
	threadId = tId;
}




/********************************************************************
 * Function:	isRead
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
bool CacheJob::isRead()
{
	return (rW == 'R');
}





/********************************************************************
 * Function:	isWrite
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
bool CacheJob::isWrite()
{
	return (rW == 'W');
}





/********************************************************************
 * Function:	getAddress
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
unsigned long long CacheJob::getAddress()
{
	return address;
}





/********************************************************************
 * Function:	getThreadId
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
int CacheJob::getThreadId()
{
	return threadId;
}




/********************************************************************
 * Function:	CacheJob Destructor
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
CacheJob::~CacheJob(void)
{
}
