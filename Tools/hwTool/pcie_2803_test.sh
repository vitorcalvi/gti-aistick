#!/bin/bash

counter=1

if [ "x$1" == "x"  ]; then
     total=1
  else
     total=$1
 fi

while [ $counter -le $total ]
do
echo $counter

./hwTool -i car2803_2017232.tb32 -s 32768


((counter++))

done
echo All done
