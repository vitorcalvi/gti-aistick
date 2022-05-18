Some notes:


1. GTI chip has different processing time for different AI models. For EUSB interface, a waiting period 
   is required before reading back the processed results from GTI chip. Please adjust the environment variable 
   GTI_EUSB_DELAY to fine-tune the FPS performance. For example, run "GTI_EUSB_DELAY=5000 ./cnnSample" for 
   gNet3 network model, and run "GTI_EUSB_DELAY=12000 ./cnnSample" for gNet1 network model

2. To help debugging, please set environment variable GTI_LOG_LEVEL for different level of debugging message. 
   "GTI_LOG_LEVEL=9" outputs most of the debugging message. The "gti.log" is created for logs as well.

3. Changing the the value of DemoFlag in Sample.cpp to 1 will run the cnnSample in demo mode.

4. Please unzip the GTI_Legacy_Models.zip file into ../../data folder before run this cnnSample program.

