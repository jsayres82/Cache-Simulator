#!/bin/bash
# Loop through calling MP2 executable storing the time.

declare -i KILOBYTE
declare -i CACHESIZE
declare -i ASSOC
declare -i BLOCKSIZE


KILOBYTE=1024

TRACE_FILE=traces/canneal.04t.longTrace
pwd

for i in 0 1 2
do
CACHESIZE=128*$KILOBYTE
BLOCKSIZE=64
ASSOC=8
for j in {1..4}
	do
	CACHESIZE=$CACHESIZE*2
		./smp_cache $CACHESIZE $ASSOC $BLOCKSIZE 4 $i  $TRACE_FILE >> variableCacheSize.csv
		
	done
done



for i in 0 1 2
do
BLOCKSIZE=64
CACHESIZE=$KILOBYTE*$KILOBYTE
ASSOC=2
for j in {1..3}
	do
	ASSOC=$ASSOC*2
		./smp_cache $CACHESIZE $ASSOC $BLOCKSIZE 4 $i  $TRACE_FILE >> variableAssoc.csv
		
	done
done



for i in 0 1 2
do
CACHESIZE=$KILOBYTE*$KILOBYTE
ASSOC=8
BLOCKSIZE=32
for j in {1..3}
	do
	BLOCKSIZE=$BLOCKSIZE*2
		./smp_cache $CACHESIZE $ASSOC $BLOCKSIZE 4 $i  $TRACE_FILE >> variableBlockSize.csv
		
	done
done

