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

./hwTool -i beach5801_52705239.tb -s 32768
if [ $? -ne 0 ]; then
    echo "test failed after $counter"
    exit -1
fi

((counter++))

done
echo All done
