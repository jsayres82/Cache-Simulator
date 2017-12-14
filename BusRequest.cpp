#include "BusRequest.h"






/********************************************************************
 * Function:	BusRequest
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
BusRequest::BusRequest(BusRequest::BusMessage busCommand, int set, int tag, int jobCycleCost, unsigned long long adr)
{
	command = busCommand;
	lineSet = set;
	lineTag = tag;
	cycleCost = jobCycleCost;
	address = adr;
	sharedLine = false;
}





/********************************************************************
 * Function:	getCommand
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
BusRequest::BusMessage BusRequest::getCommand()
{
	return command;
}






/********************************************************************
 * Function:	getSet
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
int BusRequest::getSet()
{
	return lineSet;
}





/********************************************************************
 * Function:	getTag
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
int BusRequest::getTag()
{
	return lineTag;
}





/********************************************************************
 * Function:	isLineShared
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
bool BusRequest::isLineShared()
{
	return sharedLine;
}





/********************************************************************
 * Function:	setSharedLine
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
void BusRequest::setSharedLine()
{
	sharedLine = true;
}





/********************************************************************
 * Function:	getCycleCost
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
int BusRequest::getCycleCost()
{
	return cycleCost;
}





/********************************************************************
 * Function:	BusRequest
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
BusRequest::~BusRequest(void)
{
}
