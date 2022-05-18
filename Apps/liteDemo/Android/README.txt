
This sub-directory contains the makefiles for building GTI app "liteDemo"
Four files are in this directory

Android.mk
Application.mk
build.sh
README.txt

"build.sh" is the script to run to build the application. 
32-bit Android executables will be generated. The generated binaries and 
libraries are located under sub-directory "libs/armeabi-v7a".  

To execuate the binary on Android. Please 
0) connect to your Android target (throught adb), gaining root access is required.
1) copy the contents under "armeabi-v7a" to a sub-directory under "/data/".
2) go to that directory and make sure you have read write permission for all.
3) setup library path with
   "export LD_LIBRARY_PATH=$PWD:$LD_LIBRARY_PATH"
4) run liteDemo with
   ./liteDemo <modelFile> <inputFile> [numberOfLoop]

