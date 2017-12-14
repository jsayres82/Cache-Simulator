#include "CacheSet.h"
#include "vector"
#include "CacheLine.h"
#include "CacheConstants.h"
#include "limits.h"
#include <stdio.h>




/********************************************************************
 * Function:	CacheSet
 * Inputs:		None
 * Outputs:		None
 * Description:  Container of the lines for an individual set.
 * 				Manages requests for lines and LRU eviction
 * 
 * ******************************************************************/
CacheSet::CacheSet(CacheConstants* constants,long unsigned int set)
{
	consts = constants;
	setID = set;
	
	for(int i = 0; i < (*consts).getNumLinesInSet(); i++)
	{
		CacheLine* newLine = new CacheLine(0, setID, 0);
		addLine(newLine);
		
		printf("Set Size:   %d\n",(unsigned int)allLines.size());
		
	}
	printf("Lines in Set:   %d\n",(unsigned int)(*consts).getNumLinesInSet());
}





/********************************************************************
 * Function:	isFull
 * Inputs:		None
 * Outputs:		None
 * Description: Represents a single line in the cache.
 * 
 * ******************************************************************/
bool CacheSet::isFull()
{
			
	if(allLines.size() < (*consts).getNumLinesInSet()){
		return false;
	}
	for (int i = 0; i < allLines.size(); ++i)
	{
		if (allLines[i] == NULL || ((*allLines[i]).getState() != CacheLine::modified) || ((*allLines[i]).getState() != CacheLine::sharedDirty))
		{
			return false;
		}
	}
	return true;
}



/*********************************************
 * Function:	getLRU
 * Inputs:	address
 * Outputs:	cacheLine
 * Description:  return an invalid line as LRU,
 * 			 if any, otherwise return LRU line
 * 
 * 
 * *******************************************/
void CacheSet::getLRU()
{
   int i, j, victim, min;
   char tmpstr[256];
   sprintf(tmpstr,"");
   char state[5] = {
	  'I',
	  'S',
	  'M',
	  'E',
	  'O'
  };
	 
   for (int i = 0; i < (*consts).getNumLinesInSet(); ++i)
	{
		if(i < allLines.size())
			sprintf(tmpstr, "%s%x::%x-%c-%llu\t", tmpstr, (*allLines[i]).getTag(), (*allLines[i]).getSetIndex(), state[(*allLines[i]).getState()], (*allLines[i]).lastUsedCycle);
		else
			sprintf(tmpstr, "%sNULL-NULL-NULL-NULL\t",tmpstr);      
   }   
	printf("%s\n",tmpstr);
  
}




/********************************************************************
 * Function:	replaceLine
 * Inputs:		None
 * Outputs:		None
 * Description:  Adds a new line to the Set
 * 
 * ******************************************************************/
void CacheSet::replaceLine(CacheLine* line)
{
	CacheLine* oldLine = getLRULine();
	//~ if(allLines.size() == (*consts).getNumLinesInSet())
	//~ {
		//~ if(hasInvalidLine())
		//~ {
			//~ evictLRULineInvalid();
		//~ }
		//~ else 
		//~ {
			//~ evictLRULine();		
		//~ }
	//~ }
	//~ allLines.push_back(line);

	(*oldLine).rawAddress = (*line).getAddress();
	(*oldLine).setIndex =  (*line).getSetIndex();
	(*oldLine).myTag =  (*line).getTag();
	(*oldLine).myState = CacheLine::invalid;//invalid;
	(*oldLine).lastUsedCycle =  (*consts).getCycle();
	(*oldLine).lineShared = false;
	
}



/********************************************************************
 * Function:	addLine
 * Inputs:		None
 * Outputs:		None
 * Description:  Adds a new line to the Set
 * 
 * ******************************************************************/
void CacheSet::addLine(CacheLine* line)
{
	if(allLines.size() == (*consts).getNumLinesInSet())
	{
		if(hasInvalidLine())
		{
			evictLRULineInvalid();
		}
		else 
		{
			evictLRULine();		
		}
	}
	allLines.push_back(line);
}



/********************************************************************
 * Function:	lineValid
 * Inputs:		None
 * Outputs:		None
 * Description:  True if the line is valid
 * 
 * ******************************************************************/
bool CacheSet::lineValid(long unsigned int tag)
{
	for(int i = 0; i < allLines.size(); i++)
	{
		if((allLines[i] != NULL) && ((*allLines[i]).getLineValid()))
			if(((*allLines[i]).getTag() == tag))
				return true;
	}
	return false;
}







/********************************************************************
 * Function:	hasLine
 * Inputs:		None
 * Outputs:		None
 * Description:  True if the line is valid
 * 
 * ******************************************************************/
bool CacheSet::hasLine(long unsigned int tag)
{
	for(int i = 0; i < allLines.size(); i++)
	{
		if((allLines[i] != NULL) && ((*allLines[i]).getTag() == tag) && ((*allLines[i]).getState() != CacheLine::invalid))
		{
			return true;
		}						
	}
	return false;
}








/********************************************************************
 * Function:	getLine
 * Inputs:		None
 * Outputs:		None
 * Description:  return the line with this tag,assumes line with 
 * 				that tag is in the set
 * 
 * ******************************************************************/
CacheLine* CacheSet::getLine(long unsigned int tag)
{
	if((*consts).getProtocol() != CacheConstants::DRAGON)
	{
		for(int i = 0; i < allLines.size(); i++)
		{
			if((allLines[i] != NULL) && (*allLines[i]).getTag() == tag)
			{
				//update when used
				(*allLines[i]).lastUsedCycle = (*consts).getCycle();
				return allLines[i];
			}
		}
		return NULL;
	}
	else
	{
		for(int i = 0; i < allLines.size(); i++)
		{
			if((allLines[i] != NULL) && (*allLines[i]).getTag() == tag)
			{
					(*allLines[i]).lastUsedCycle = (*consts).getCycle();
						return allLines[i];
			}
		}
	}
	return NULL;
}










/********************************************************************
 * Function:	evictLineModified
 * Inputs:		None
 * Outputs:		None
 * Description:  True if oldest line is Modified state
 * 
 * ******************************************************************/
bool CacheSet::evictLineModified()
{
	int lineToEvict =-1;
	unsigned long long leastRecentCycle = ULLONG_MAX;

	if (allLines.size() != (*consts).getNumLinesInSet())
		return false;


	for (int i = 0; i < allLines.size(); ++i)
	{
		if ((allLines[i] != NULL) && (*allLines[i]).lastUsedCycle < leastRecentCycle)
		{
			leastRecentCycle = (*allLines[i]).lastUsedCycle;
			lineToEvict = i;
		}
	}
	if(lineToEvict >= 0)
	{
		if((*allLines[lineToEvict]).getState() == CacheLine::modified || (*allLines[lineToEvict]).getState() == CacheLine::sharedDirty)
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}










/********************************************************************
 * Function:	evictLineShared
 * Inputs:		None
 * Outputs:		None
 * Description:  True if the oldest line is shared
 * 
 * ******************************************************************/
bool CacheSet::evictLineShared()
{
	int lineToEvict = -1;
	unsigned long long leastRecentCycle = ULLONG_MAX;

	if (allLines.size() != (*consts).getNumLinesInSet())
		return false;
		

	for (int i = 0; i < allLines.size(); ++i)
	{
		if ((allLines[i] != NULL) && (*allLines[i]).lastUsedCycle < leastRecentCycle)
		{
			leastRecentCycle = (*allLines[i]).lastUsedCycle;
			lineToEvict = i;
		}
	}
	
	if(lineToEvict >= 0)
	{
		if((*allLines[lineToEvict]).getState() == CacheLine::shared  || (*allLines[lineToEvict]).getState() == CacheLine::sharedClean){
			return true;
		}
	}
	else
	{
		return false;
	}
}










/********************************************************************
 * Function:	allLinesFull
 * Inputs:		None
 * Outputs:		None
 * Description:  True if all lines have been used(lines could be invalid)
 * 
 * ******************************************************************/
bool CacheSet::allLinesFull(){
	if (allLines.size() != (*consts).getNumLinesInSet())
		return false;
	else
		return true;
}












/********************************************************************
 * Function:	ShowCache
 * Inputs:		None
 * Outputs:		None
 * Description:  True if there is an invalid line in the set
 * 
 * ******************************************************************/
 bool CacheSet::hasInvalidLine()
{
	for (int i = 0; i < allLines.size(); ++i)
	{
		if((*allLines[i]).getState() == CacheLine::invalid)
			return true;
	}
	return false;
}












/********************************************************************
 * Function:	evictLineInvalid
 * Inputs:		None
 * Outputs:		None
 * Description:  true if the line we're evicting is modified, false 
 * 				otherwise
 * 
 * ******************************************************************/
 bool CacheSet::evictLineInvalid()
 {
	int lineToEvict = -1;
	unsigned long long leastRecentCycle = ULLONG_MAX;

	if (allLines.size() != (*consts).getNumLinesInSet())
		return false;

	for (int i = 0; i < allLines.size(); ++i)
	{
		if ((allLines[i] != NULL) && (*allLines[i]).lastUsedCycle < leastRecentCycle)
		{
			leastRecentCycle = (*allLines[i]).lastUsedCycle;
			lineToEvict = i;
		}
	}
	
	if(lineToEvict >= 0)
	{
		if((*allLines[lineToEvict]).getState() == CacheLine::invalid)
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}












/********************************************************************
 * Function:	evictLRULineInvalid
 * Inputs:		None
 * Outputs:		None
 * Description:  Remove the oldest Invalid block in the set
 * 
 * ******************************************************************/
void CacheSet::evictLRULineInvalid()
{
	unsigned long long leastRecentCycle = ULLONG_MAX;
	int lineToEvict= -1;

	  char state[5] = {
	  'I',
	  'S',
	  'M',
	  'E',
	  'O'
  };
  
	for (int i = 0; i < allLines.size(); ++i)
	{
		if((allLines[i] != NULL) && ((*allLines[i]).getState() == CacheLine::invalid))
		{
			(*allLines[lineToEvict]).setState(CacheLine::invalid);
				//~ allLines.erase(allLines.begin() + i);
		}
	}
	
}









/********************************************************************
 * Function:	evictLine
 * Inputs:		None
 * Outputs:		None
 * Description:  Remove a specific block line in the set
 * 
 * ******************************************************************/
void CacheSet::evictLine(long unsigned int tag)
{
	for(int i = 0; i < allLines.size(); i++)
	{
		if((allLines[i] != NULL) && (*allLines[i]).getTag() == tag)
			//~ allLines.erase(allLines.begin() + i);
			(*allLines[i]).setState(CacheLine::invalid);
	}
}






/********************************************************************
 * Function:	evictLRULine
 * Inputs:		None
 * Outputs:		None
 * Description:  Remove the oldest line in the set
 * 
 * ******************************************************************/
CacheLine* CacheSet::getLRULine()
{
	unsigned long long leastRecentCycle = ULLONG_MAX;
	int lineToEvict=-1;
	
	for (int i = 0; i < allLines.size(); ++i)
	{
		if((*allLines[i]).getState() == CacheLine::invalid)
		{
			//~ allLines.erase(allLines.begin() + i);
			return allLines[i];
		}
		
		if ((allLines[i] != NULL) && (*allLines[i]).lastUsedCycle < leastRecentCycle)
		{
			leastRecentCycle = (*allLines[i]).lastUsedCycle;
			lineToEvict = i;
		}
	}
	if(lineToEvict >= 0)
	{
		return allLines[lineToEvict];
		//~ allLines.erase(allLines.begin() + lineToEvict);
	}
		
}


/********************************************************************
 * Function:	evictLRULine
 * Inputs:		None
 * Outputs:		None
 * Description:  Remove the oldest line in the set
 * 
 * ******************************************************************/
void CacheSet::evictLRULine()
{
	unsigned long long leastRecentCycle = ULLONG_MAX;
	int lineToEvict=-1;
	
	for (int i = 0; i < allLines.size(); ++i)
	{
		//~ if((*allLines[i]).getState() == CacheLine::invalid)
		//~ {
			//~ allLines.erase(allLines.begin() + i);
			//~ return;
		//~ }
		
		if ((allLines[i] != NULL) && (*allLines[i]).lastUsedCycle < leastRecentCycle)
		{
			leastRecentCycle = (*allLines[i]).lastUsedCycle;
			lineToEvict = i;
		}
	}
	if(lineToEvict >= 0)
	{
		(*allLines[lineToEvict]).setState(CacheLine::invalid);
		//~ allLines.erase(allLines.begin() + lineToEvict);
	}
		
}
//~ 
//~ 
//~ /*upgrade LRU line to be MRU line*/
//~ void Cache::updateLRU(cacheLine *line)
//~ {
  //~ line->setSeq(currentCycle);  
//~ }
//~ 



//~ /*return an invalid line as LRU, if any, otherwise return LRU line*/
//~ cacheLine * Cache::getLRU(ulong addr)
//~ {
   //~ ulong i, j, victim, min;
//~ 
   //~ victim = assoc;
   //~ min    = currentCycle;
   //~ i      = calcIndex(addr);
   //~ 
   //~ for(j=0;j<assoc;j++)
   //~ {
      //~ if(cache[i][j].isValid() == 0) return &(cache[i][j]);     
   //~ }   
   //~ for(j=0;j<assoc;j++)
   //~ {
	 //~ if(cache[i][j].getSeq() <= min) { victim = j; min = cache[i][j].getSeq();}
   //~ } 
   //~ assert(victim != assoc);
   //~ 
   //~ return &(cache[i][victim]);
//~ }
//~ 
//~ 



//~ /*find a victim, move it to MRU position*/
//~ cacheLine *Cache::findLineToReplace(ulong addr)
//~ {
   //~ cacheLine * victim = getLRU(addr);
   //~ updateLRU(victim);
  //~ 
   //~ return (victim);
//~ }
//~ 
//~ 



//~ /*allocate a new line*/
//~ cacheLine *Cache::fillLine(ulong addr)
//~ { 
   //~ ulong tag;
  //~ 
   //~ cacheLine *victim = findLineToReplace(addr);
   //~ assert(victim != 0);
   //~ if(victim->getFlags() == DIRTY) writeBack(addr);
    	//~ 
   //~ tag = calcTag(addr);   
   //~ victim->setTag(tag);
   //~ victim->setFlags(VALID);    
   //~ /**note that this cache line has been already 
      //~ upgraded to MRU in the previous function (findLineToReplace)**/
//~ 
   //~ return victim;
//~ }
//~ 


/*look up line*/
//~ cacheLine * Cache::findLine(ulong addr)
//~ {
   //~ ulong i, j, tag, pos;
   //~ 
   //~ pos = assoc;
   //~ tag = calcTag(addr);
   //~ i   = calcIndex(addr);
  //~ 
   //~ for(j=0; j<assoc; j++)
	//~ if(cache[i][j].isValid())
	    //~ if(cache[i][j].getTag() == tag)
		//~ {
		     //~ pos = j; break; 
		//~ }
   //~ if(pos == assoc)
	//~ return NULL;
   //~ else
	//~ return &(cache[i][pos]); 
//~ }



/********************************************************************
 * Function:	CacheSet
 * Inputs:		None
 * Outputs:		None
 * Description:  Prints all lines and ways of the cache
 * 
 * ******************************************************************/
CacheSet::~CacheSet(void)
{
}
