/***********************************************************************
*
* Copyright (c) 2017-2019 Gyrfalcon Technology Inc. All rights reserved.
* See LICENSE file in the project root for full license information.
*
************************************************************************/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#include <filesystem>
#endif
#include <time.h>
#include <cstring>
#include <functional>
#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>

#ifdef LINUX
#include <unistd.h>
#include <pthread.h>

#define fread_s(buffer, bufferSize, elementSize, count, stream) fread(buffer, elementSize, count, stream)

typedef unsigned int DWORD;
typedef unsigned char BYTE;
typedef char TCHAR;
typedef int  BOOL;
#define TRUE    1
#define FALSE   0
#endif
#ifdef WIN32
#define pthread_t HANDLE
#define pthread_create(tid, attr, func, arg) (*tid = CreateThread(attr, 0, (LPTHREAD_START_ROUTINE )func, (LPVOID)arg, 0, NULL))
#define pthread_join(tid, delaytime) do{ \
	WaitForSingleObject(tid, INFINITE);  \
	CloseHandle(tid);  \
	}while(0)
#endif

using std::string;
using std::vector;

#define GTI_IMAGE_OUT_LENGTH            0x00008000
#define GTI_LEARN_OUT_LENGTH            0x00870000
#define GTI_LEARN_UNPACK_OUT_LENGTH     0x00CEB800
#define GTI_LEARN_OUT_LENGTH_L5         0x00010000
#define GTI_LEARN_UNPACK_OUT_LENGTH_L5  0x00018800

typedef std::pair<string, float> GTIPrediction;

typedef struct _fc_thread_data
{
    float *firstAddress;
    int firstLen;
    float *secondAddress;
    int secondLen;
    float *thirdAddress;
    int thirdLen;
    float *sumBuffer;
} FC_THREAD_DATA, *pFC_THREAD_DATA;

class Classify
{
public:
    Classify(char *CoefDataFcFileName, char *LabelFileName);
    ~Classify();

    int GtiClassify(float *InputData, int Count);
    char *GtiGetPredicationString(int Index);
    int GtiGetPredicationSize();

    float *GtiGetFeatureVector();
    DWORD GtiGetFeatureVectorLength();

private:
    int m_CPU_num = 1;
    int m_CoefDataLoad = 0;

    // varible for classify.
    DWORD m_Fc6_input_size;
    DWORD m_Fc6_output_size;
    DWORD m_Fc7_input_size;
    DWORD m_Fc7_output_size;
    DWORD m_Fc8_input_size;
    DWORD m_Fc8_output_size;
    float *m_Fc6input = NULL;
    float *m_Fc6coef = NULL;
    float *m_Fc6bias = NULL;
    float *m_Fc7input = NULL;
    float *m_Fc7coef = NULL;
    float *m_Fc7bias = NULL;
    float *m_Fc8input = NULL;
    float *m_Fc8coef = NULL;
    float *m_Fc8bias = NULL;
    float *m_Fc6_out = NULL;
    float *m_Fc7_out = NULL;
    float *m_Fc8_out = NULL;
    float *m_Probs_out = NULL;
    float *m_Layer6_yi = NULL;
    float *m_Layer6_coef = NULL;

    vector<FC_THREAD_DATA>m_Fc6ThreadIn;
    vector<FC_THREAD_DATA>m_Fc7ThreadIn;
    vector<FC_THREAD_DATA>m_Fc8ThreadIn;
    vector<float *>m_Layer6_sum;
    vector<string> m_Labels;
    vector<GTIPrediction> m_Predications;
    string m_PredText;

    template <class T>
    void safeRelease(T *buffer)
    {
        if (buffer)
        {
            delete [] buffer;
        }
    }
    void gtiLoadLabelFile(char *FileName);
    int gtiPreloadCoefData(const char *CoefDataFcFileName);
    vector<int> gtiArgmax(float *FcOut, unsigned int FcOutLen, int N);
    float* gtiAccelerateFC(unsigned int *FcOutLen);
};


