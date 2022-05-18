Gyrfalcon Technology Inc. 
GTISDK 4.x Release

To run a quick demo, please connect to a GTI 5801 device
to your Linux Ubuntu 16.04 PC, use the following commands in a commandline 
console:
  source SourceMe.env
  liteDemo Apps/Models/5801/gti_mnet224_fc40_5801.model Apps/Data/Image_bmp_c40/Geyser.jpg

To build and run a simple application:
  source SourceMe.env
  cd Apps/liteDemo
  make
  ./liteDemo ../Models/5801/gti_mnet224_fc40_5801.model ../Data/Image_bmp_c40/Geyser.jpg

For detailed documentations about this SDK package, please see
  Documents/GTI2801-2803-5801_SDK_v4.2.pdf

Please note that setting up the correct library path is required. Command
  export LD_LIBRARY_PATH="${PWD}/Lib/Linux/<arch>:$LD_LIBRARY_PATH" 
                         (<arch> is the archtecture of the target system)
sets up the "LD_LIBRARY_PATH" environmental variable so applications can load 
libraries with the correct version.


