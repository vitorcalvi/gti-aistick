/***********************************************************************
*
* Copyright (c) 2017-2019 Gyrfalcon Technology Inc. All rights reserved.
* See LICENSE file in the project root for full license information.
*
************************************************************************/

 // GTI2801 simple sample code. It demonstrates how to use GTI library
 //APIs to access GTI2801 Chip.
 
#include "stdafx.h"
#include "liteSample.hpp"
#include "GtiClassify.h"
#ifdef WIN32
#include "strsafe.h"
#endif
#include "GtiLog.h"

using std::string;

// Commenting out this define to enable SDK 4.x API
#define GTISDK3
/* This deine enables a demo to show case the SDK 4.x API equivalence

   Before commenting out above define to test the API, please prepare
   your model in the following way:

mkdir ../../Apps/Models/2801/gnet2_3/  # crate a new dir for the new model
cd ../../Apps/Models/2801/gnet2_3/
cp ../../../../Tools/modelTool/modelTool . #get the model conversion tool
cp /usr/local/GTISDKPATH/data/Models/gti2801/gnet2_3/cnn_3/gnet32_coef_512.dat gti_gnet32_coef_512.model.1
cat << EOF > gti_gnet32_coef_512.model.0
{
   "name": "gti_gnet32_coef_512",
   "version": "4.0.0",
   "data hash": null,
   "layer": [
        {
            "data file": "gti_gnet32_coef_512.model.1",
            "data offset": 0,
            "device": {
                "chip": "2801",
                "emmc delay": 15000,
                "name": null,
                "type": 0
            },
            "name": "cnn",
            "operation": "GTICNN",
            "output channels": 256,
            "output height": 7,
            "output width": 7
        }
    ]
}
EOF
./modelTool modelenc gti_gnet32_coef_512.model.0 # create a new model with name gti_gnet32_coef_512.model.0.gti
cp /usr/local/GTISDKPATH/data/Models/gti2801/gnet2_3/fc/gnet32_fc128_20class.bin .
cp /usr/local/GTISDKPATH/data/Models/gti2801/gnet2_3/fc/gnet32_fc128_20class_label.txt . # copy fc data files
*/


string glGtiPath = "GTISDKPATH";

#ifdef WIN32
string dataPath = "\\data\\";
// Image root
string glImageRoot = dataPath + "Image_Lite\\";
string glImagename = glImageRoot + "bridge.bin";

#ifdef GTISDK3
string glGnet2_3Root = dataPath + "Models\\gti2801\\gnet2_3\\";

// FC model and label file for gnet32 image classification
string glPicCoefDataFcGnet32 = glGnet2_3Root + "fc\\gnet32_fc128_20class.bin";
string glPicLabelGnet32 = glGnet2_3Root + "fc\\gnet32_fc128_20class_label.txt";

// Gnet32 CNN coef model file
string glGnet32Coef512 = glGnet2_3Root + "cnn_3\\gnet32_coef_512.dat";
// CNN model and EUSB interface configuration file
string glGnet32UserInput = glGnet2_3Root + "cnn_3\\userinput.txt";
#endif
#else
string dataPath = "/data/";

// Image root
string glImageRoot = dataPath + "Image_Lite/";
string glImagename = glImageRoot + "bridge.bin";

#ifdef GTISDK3
string glGnet2_3Root = dataPath + "Models/gti2801/gnet2_3/";

// FC model and label file for gnet32 image classification
string glPicCoefDataFcGnet32 = glGnet2_3Root + "fc/gnet32_fc128_20class.bin";
string glPicLabelGnet32 = glGnet2_3Root + "fc/gnet32_fc128_20class_label.txt"; 

// CNN model and EUSB interface configuration file
string glGnet32Coef512 = glGnet2_3Root + "cnn_3/gnet32_coef_512.dat";
string glGnet32UserInput = glGnet2_3Root + "cnn_3/userinput.txt";
#endif
#endif

#ifdef GTISDK3
#else
// redefine the model path to the newly created model
string glGnet2_3Root = "../../Apps/Models/2801/gnet2_3/";
string glPicCoefDataFcGnet32 = glGnet2_3Root + "gnet32_fc128_20class.bin";
string glPicLabelGnet32 = glGnet2_3Root + "gnet32_fc128_20class_label.txt"; 
string glGnet32Model = glGnet2_3Root + "gti_gnet32_coef_512.model.0.gti";
#endif

string glDevice = "eusb";

int gChgImage = 0, gChgCnn = 0, gChgFc = 0, gChgLabel = 0, gChgConfig = 0;

int main(int argc, _TCHAR* argv[])
{
#ifdef LINUX
    int opt;
    if(argc < 2)
    {
        printf("Usage:\n");
        printf("\t ./liteSample -c CNN_Model -f FC_Model -l Lable_File -i Input_Image_File -u Network_Config_File -d device (eusb or ftdi or pcie)\n");
    }

    while ((opt = getopt(argc, argv, "hcfliud")) != -1) 
    {
       switch (opt) 
       {
           case 'h':
               printf("Usage:\n");
               printf("\t ./liteSample -c CNN_Model -f FC_Model -l Lable_File -i Input_Image_File -u Network_Config_File -d (eusb or ftdi or pcie)\n");
               exit(0);

#ifdef GTISDK3
           case 'c':
                glGnet32Coef512 = (argv[optind]);
                gChgCnn = 1;
                break;

           case 'f':
                glPicCoefDataFcGnet32 = (argv[optind]);
                gChgFc = 1;    
               break;
#endif
            case 'l':
                glPicLabelGnet32 = (argv[optind]);
                gChgLabel = 1;             
               break;

           case 'i':
                glImagename = (argv[optind]);
                gChgImage = 1;
               break;

#ifdef GTISDK3
           case 'u':
                glGnet32UserInput = (argv[optind]); 
                gChgConfig = 1;                 
               break;

           case 'd':
                glDevice = (argv[optind]);               
               break;
#endif
           default: /* '?' */
               exit(EXIT_FAILURE);
       }
   }
#endif

    char *devicename = (char *)"0";

    char *logLevel=getenv("GTI_LOG_LEVEL");
    if(logLevel) GtiLog::setLevel(static_cast<GTI_LOG_LEVEL>(atoi(logLevel)));

    LiteSamples test(devicename);

    test.Test_picture();
    return 0;
}

//====================================================================
// Function name: LiteSamples()
// Class Samples construction.
// This function calls GTI library APIs to set device type, open device,
// initialize GTI SDK library. It also allocates memory for the sample
// code.
// This sample code uses Gnet3.
//====================================================================
LiteSamples::LiteSamples(char *deviceName)
{
    char *gtiHome = NULL;

    m_deviceName = deviceName;

    gtiHome = getenv(glGtiPath.c_str());
    if (gtiHome == NULL)
    {
        printf("GTISDK home path is not set! Please see GTI SDK user guide.\n");
        exit(0);
    }

    m_SdkPath = string(gtiHome);

    m_ImageName     = (gChgImage == 1) ? glImagename : m_SdkPath + glImagename;
#ifdef GTISDK3
    m_coefName      = (gChgCnn == 1) ? glGnet32Coef512 : m_SdkPath + glGnet32Coef512;
    m_userConfig    = (gChgConfig == 1) ? glGnet32UserInput : m_SdkPath + glGnet32UserInput;
    m_picFcname     = (gChgFc == 1) ? glPicCoefDataFcGnet32 : m_SdkPath + glPicCoefDataFcGnet32;
    m_picLabelname  = (gChgLabel == 1) ? glPicLabelGnet32 : m_SdkPath + glPicLabelGnet32;

    // Create GTI device
    if(glDevice == "eusb" || glDevice == "EUSB")
       m_Device = GtiDeviceCreate(EUSB, (char *)m_coefName.c_str(), (char *)m_userConfig.c_str());
    else if(glDevice == "ftdi" || glDevice == "FTDI")
        m_Device = GtiDeviceCreate(FTDI, (char *)m_coefName.c_str(), (char *)m_userConfig.c_str());
    else if(glDevice == "pcie" || glDevice == "PCIE")
        m_Device = GtiDeviceCreate(PCIE, (char *)m_coefName.c_str(), (char *)m_userConfig.c_str());
    else
    {
        printf("The device name is wrong, exit...\n");
        exit(0);
    }

    // Open device
    GtiOpenDevice(m_Device, deviceName);

    // Initialize GTI SDK
    if (!GtiInitialization(m_Device))
    {
        std::cout << "GTI initialization failed." << std::endl;
    }

    // Allocate memory for sample code use
    m_ImageOutPtr = new float[GtiGetOutputLength(m_Device)];
    if (!m_ImageOutPtr)
    {
        std::cout << "GTI allocation (m_ImageOutPtr) failed." << std::endl;      
    }
#else
    m_picFcname     = glPicCoefDataFcGnet32;
    m_picLabelname  = glPicLabelGnet32;

    std::cout<< "Opening Model File:" << glGnet32Model << std::endl;
    //Load GTI image classification model
    m_model=GtiCreateModel( glGnet32Model.c_str() );
    if (!m_model)
    {
        std::cout << "GTI model initialization failed." << std::endl;
    }
#endif

    m_Img224x224x3Buffer = new BYTE[GTI_IMAGE_WIDTH * GTI_IMAGE_HEIGHT * GTI_IMAGE_CHANNEL];
    if (!m_Img224x224x3Buffer)
    {
        std::cout << "GTI allocation (m_Img224x224x3Buffer) failed." << std::endl;
    }
}

//====================================================================
// Function name: ~LiteSamples()
// Class Samples destruction.
// This function releases the memory allcated in Samples(), 
// it also calls GTI library API to close device.
//====================================================================
LiteSamples::~LiteSamples()
{
    safeRelease(m_ImageOutPtr);
    safeRelease(m_Img224x224x3Buffer);
#ifdef GTISDK3

    // Close device
    GtiCloseDevice(m_Device);
    // Release device
    GtiDeviceRelease(m_Device);
#endif
}

//====================================================================
// Function name: Test_picture()
//
// This function reads image from an image file with the format
// 224 x 224 pixels x 3 channels, sends the image data to GTI chip
// and gets the CNN result data from chip, classifying the result.
// Expected output:
// 0.999955 - 17 : 17 bridge
// 4.50576e-05 - 9 : 9 unknown
// 1.57612e-10 - 16 : 16 pagoda
// 1.85889e-13 - 0 : 0 hotel
//  7.35737e-14 - 18 : 18 helicopter
//====================================================================
void LiteSamples::Test_picture()
{
    int imageInLength = 224*224*3;
    char *imagename = (char *)m_ImageName.c_str();
    int unused;
    FILE *fp_r;
    (void)unused;

    std::cout << "Opening files: fc data:" << m_picFcname << "   label:"<< m_picLabelname << std::endl;
    Classify* gtiClassify = GtiClassifyCreate(m_picFcname.c_str(), m_picLabelname.c_str());

    std::cout << "---------- Prediction for " << imagename << " ----------" << std::endl;

    fp_r = fopen(imagename, "rb");
    if (fp_r == NULL)
    {
        std::cout << "File is not found." << imagename << std::endl;
        GtiClassifyRelease(gtiClassify);
        return;
    }
    fseek(fp_r,0,SEEK_END);
    int size = ftell(fp_r);
    if(size != (224 * 224 * 3))
    {
        std::cout << "---*" << imagename << "* has wrong image format. The image must be in 224 x 224 x 3 plannar BGR raw data format." << std::endl;
        std::cout << "--- Please use the tool under 'Tools/image_tool' to convert the format." << std::endl;
        exit(0);
    }

    fseek(fp_r,0,SEEK_SET);

    unused = fread(m_Img224x224x3Buffer, 1, imageInLength, fp_r);
    fclose(fp_r);

#ifdef GTISDK3
    int ret = GtiHandleOneFrameFloat(m_Device, m_Img224x224x3Buffer, imageInLength, m_ImageOutPtr, GtiGetOutputLength(m_Device));
    if (!ret)
    {
        std::cout << "Handle one frame error!" << std::endl;
        GtiClassifyRelease(gtiClassify);
        return;
    }
#else
    GtiTensor tensor;
    tensor.width=GTI_IMAGE_WIDTH; tensor.height=GTI_IMAGE_HEIGHT; tensor.depth=GTI_IMAGE_CHANNEL; 
    tensor.stride = 1; tensor.format = TENSOR_FORMAT_BINARY;
    tensor.size = tensor.width * tensor.height * tensor.depth ; // 8bit per channel
    tensor.buffer = (void*)m_Img224x224x3Buffer;
    tensor.tag = nullptr;

    GtiTensor *tensorOut=GtiEvaluate( m_model, &tensor);
    if(tensorOut==nullptr)
    {
        std::cout << "Evaluate one frame error!" << std::endl;
        GtiClassifyRelease(gtiClassify);
        return;
    }
    std::cout<< "Completed Evaluation: buffer=" << std::showbase << std::hex << tensorOut->buffer << std::endl;
    std::cout<< "Dimensions : " << tensorOut->width << " " << tensorOut->height << " " << tensorOut->depth << " " << std::endl;
    m_ImageOutPtr = (float*)tensorOut->buffer;
    std::cout<< "Data : "<< std::showbase << std::hex <<m_ImageOutPtr[0]<<" "<<m_ImageOutPtr[1]<<" "<<m_ImageOutPtr[2]<<" "<<m_ImageOutPtr[3] << std::endl;

#endif

    GtiClassifyFC(gtiClassify, m_ImageOutPtr, 5);

    /* Print the top N predictions. */
    for (int i = 0; i < 5; ++i)
    {
        char *ptext = GetPredicationString(gtiClassify, i);
        std::cout << std::fixed << std::setprecision(4) << ptext << std::endl;
    }

    std::cout << "End of process image " << m_filename << std::endl;
    std::cout << "press Enter to exit" ;
    getchar();
    GtiClassifyRelease(gtiClassify);
}
