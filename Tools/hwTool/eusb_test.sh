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

./hwTool -i bridge2801_245658880_14047216_32833464.tb -s 131072
#./hwTool -i fc_2801.tb -s 131072
#./hwTool -i testio_2801.tb -s 2097152
#./hwTool -i learn_2801.tb -s 35389440

((counter++))

done
echo All done
