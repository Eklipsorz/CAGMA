#!/bin/bash 

function testdir()
{
	expTime=5
	begin=`date +%s`
	diff=0
	
	while [ "$diff" -le "$expTime" ]
	do

			
		end=`date +%s`
		diff=$((end-begin))
	done

}


for i in $(seq 1 5)
do
	echo $i	

done
