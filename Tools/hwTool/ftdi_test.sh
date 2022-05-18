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

./hwTool -i bridge2801_245658880_14047216_32833464.tb -s 65536

((counter++))

done
echo All done
