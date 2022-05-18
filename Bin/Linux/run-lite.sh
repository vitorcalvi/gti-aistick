#!/bin/bash

function usage()
{
    cat <<HERE
This is the command to test GTISDK and GTI hardward functions.

Usage: $0 <-c CNN_Model -f FC_Model -l Lable_File -i Input_Image_File -u Network_Config_File>
       
       You need to setup GTISDKPATH to piont out the path of data/Model file.
       In this test command, GTISDKPATH was setup to GTISDK root path.

HERE
}

#setup default
GTISDKROOT=$PWD/../../
export CPU_ARCH=armv7l
export OS_ARCH=Linux

if [ -z ${GTISDKPATH} ]; then
    export GTISDKPATH=$GTISDKROOT
fi    

#setup library path
export LD_LIBRARY_PATH=$GTISDKROOT/Lib/${OS_ARCH}/${CPU_ARCH}:$LD_LIBRARY_PATH
./${CPU_ARCH}/liteSample ${*}

