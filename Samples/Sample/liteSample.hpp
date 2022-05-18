/***********************************************************************
*
* Copyright (c) 2017-2019 Gyrfalcon Technology Inc. All rights reserved.
* See LICENSE file in the project root for full license information.
*
************************************************************************/

#pragma once

#include <iomanip>

#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <time.h>
#include <cstring>
#include <functional>
#include "Classify.hpp"
#include "GTILib.h"

#ifdef LINUX
#include <unistd.h>
#include <sys/time.h>

#define Sleep(ms) usleep(ms * 1000)

#define min(a, b) (((a)<(b)) ? (a) : (b))

#define scanf_s scanf
#define fscanf_s fscanf
#define sprintf_s sprintf

typedef unsigned int DWORD;
typedef unsigned char BYTE;
typedef char TCHAR, _TCHAR;
typedef int  BOOL;
typedef unsigned long ULONGLONG;
#endif

using std::string;

#define FTDI                0
#define EUSB                1
#define PCIE                2

#define GTI_IMAGE_WIDTH     224
#define GTI_IMAGE_HEIGHT    224
#define GTI_IMAGE_CHANNEL   3

class LiteSamples
{
public:
    LiteSamples(char *deviceName);
    ~LiteSamples();

    void Test_picture();

protected:
    char *m_deviceName;
    string m_coefName;
    string m_picFcname;
    string m_picLabelname;

    string m_ImageName;
    string m_SdkPath;
    string m_filename;
    string m_userConfig;

    GtiDevice *m_Device;
    BYTE *m_Img224x224x3Buffer = NULL;
    float *m_ImageOutPtr = NULL;

    template <class T>
    void safeRelease(T& buffer)
    {
        if (buffer)
        {
            delete [] buffer;
            buffer = NULL;
        }
    }

    GtiModel * m_model;
};

