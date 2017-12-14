CC = g++
OPT = -O3
OPT = -g -O3
WARN = -Wall
ERR = 
DEBUG = 

ifeq ($(debug),1)
	DEBUG = -DDATADEBUG=1
endif

CFLAGS = $(OPT) $(DEBUG) $(WARN) $(ERR) $(INC) $(LIB)

SIM_INC =  CacheStats.h CacheSet.h CacheLine.h  CacheJob.h CacheController.h  CacheConstants.h Cache.h BusRequest.h  AtomicBusManager.h 
  
SIM_SRC = CacheStats.cpp CacheSet.cpp CacheLine.cpp  CacheJob.cpp CacheController.cpp  CacheConstants.cpp Cache.cpp BusRequest.cpp  AtomicBusManager.cpp 
		
SIM_OBJ = CacheStats.o CacheSet.o CacheLine.o CacheJob.o CacheController.o CacheConstants.o Cache.o BusRequest.o AtomicBusManager.o
              
LINK_OBJ =  AtomicBusManager.o    BusRequest.o    Cache.o    CacheConstants.o    CacheController.o    CacheJob.o    CacheLine.o    CacheSet.o    CacheStats.o 

LIBS_AUTO =

LIBS = $(LIBS_AUTO) $(LINK_OBJ)

all: smp_cache
	@echo "Compilation Done ---> nothing else to make :) "

smp_cache: $(SIM_OBJ)
	$(CC) -o smp_cache $(CFLAGS) $(SIM_OBJ) $(SIM_INC) -lm 


	@echo $(CFLAGS)
	@echo "----------------------------------------------------------"
	@echo "-----------FALL15-506 SMP SIMULATOR (SMP_CACHE)-----------"
	@echo "----------------------------------------------------------"
 
.cc.o:
	$(CC) $(CFLAGS) -c $*.cpp

	
	
	
clean:
	rm -f *.o smp_cache

clobber:
	rm -f *.o


