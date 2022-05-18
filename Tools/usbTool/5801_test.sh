
counter=1

if [ "x$1" == "x"  ]; then
     total=1
  else
     total=$1
 fi

while [ $counter -le $total ]
do
echo $counter

./usbTool beach_52705239.bin beach_out_golden.bin 

if [ $? -ne 0 ]; then
    echo "test failed after $counter"
    exit -1
fi

((counter++))

done
echo All done
