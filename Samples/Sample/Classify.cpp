/***********************************************************************
*
* Copyright (c) 2017-2019 Gyrfalcon Technology Inc. All rights reserved.
* See LICENSE file in the project root for full license information.
*
************************************************************************/

// GTI2801 output result classify sample code.

#include "stdafx.h"
#include "Classify.hpp"
#include "GtiClassify.h"
#include <math.h>
#ifdef ANDROID
#include <android/log.h>
#define  LOG_TAG    "GTI_JNI"
#define  ALOG(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#endif

#include<sstream>
template <typename T>
static string to_string(T value)
{
  //create an output string stream
  std::ostringstream os ;

  //throw the value into the string stream
  os << value ;

  //convert the string stream into a string and return
  return os.str() ;
}

//=======================================================================
// Function name: Classify(char *CoefDataFcFileName, char *LabelFileName)
// This is Classify construct function, it loads coef filter FC file and label file.
//
// Input: char *CoefDataFcFileName - coef filter FC file name.
//        char *LabelFileName - label file name.
// Return: none.
//=======================================================================
Classify::Classify(char *CoefDataFcFileName, char *LabelFileName)
{
    // Get CPU number
#ifdef WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    m_CPU_num = sysinfo.dwNumberOfProcessors;
#endif
#ifdef LINUX
    m_CPU_num = sysconf(_SC_NPROCESSORS_ONLN);
#endif
    if (m_CPU_num > 8)
    {
        m_CPU_num /= 2;
    }

    // Load label file and Coef data file
    gtiLoadLabelFile(LabelFileName);
    m_CoefDataLoad = gtiPreloadCoefData(CoefDataFcFileName);
}

//=======================================================================
// Function name: ~Classify()
// This is Classify destruct function, it frees the memory for Classify.
//
// Input: none.
// Return: none.
//=======================================================================
Classify::~Classify()
{
    safeRelease(m_Fc6input);
    safeRelease(m_Fc6coef);
    safeRelease(m_Fc6bias);
    safeRelease(m_Fc7input);
    safeRelease(m_Fc7coef);
    safeRelease(m_Fc7bias);
    safeRelease(m_Fc8input);
    safeRelease(m_Fc8coef);
    safeRelease(m_Fc8bias);
    safeRelease(m_Fc6_out);
    safeRelease(m_Fc7_out);
    safeRelease(m_Fc8_out);
    safeRelease(m_Probs_out);
    safeRelease(m_Layer6_yi);
    safeRelease(m_Layer6_coef);
    for (unsigned int i = 0; i < m_Layer6_sum.size(); i++)
    {
        safeRelease(m_Layer6_sum[i]);
    }
    m_Layer6_sum.clear();
    m_Predications.clear();
    m_Fc6ThreadIn.clear();
    m_Fc7ThreadIn.clear();
    m_Fc8ThreadIn.clear();
}

//=======================================================================
// Function name: int GTIClassify(float *InputData, int Count)
// This function analyzes the results of GTI2801 chip, classifies the
// image type, gets a vector with format {image name, index}, returns the sub
// mode type.
//
// Input: float *InputData - input data buffer for FC.
//        int Count - how many predication vectors to be output.
// Return: int - >= 0: sub mode type.
//                 -1: failed.
//=======================================================================
int Classify::GtiClassify(float *InputData, int Count)
{
    int num;
    int index;
    vector<int> maxN;
    vector<float> output;
    string lable_id;
    float *fcOut = NULL;
    unsigned int fcOutLen = 0;
 
    if (m_Predications.size())
    {
        m_Predications.clear();
    }

    for (unsigned int i = 0; i < m_Fc6_input_size; i++)
    {
        m_Fc6input[i] = InputData[i];
    }

    fcOut = gtiAccelerateFC(&fcOutLen);

    num = std::min<int>((int)m_Labels.size(), Count);
    maxN = gtiArgmax(fcOut, fcOutLen, num);
    for (int i = 0; i < num; ++i)
    {
        index = maxN[i];
        lable_id = to_string(index) + " : " + m_Labels[index];
        m_Predications.push_back(std::make_pair(lable_id, fcOut[index]));
    }

    return maxN[0];
}

//====================================================================
// Function name: void gtiLoadLabelFile(char *FileName)
// This function loads label file.
//
// Input: char *FileName - label file name.
// Return: None.
//====================================================================
void Classify::gtiLoadLabelFile(char *FileName)
{
    string line;

    if (FileName == NULL)
    {
        printf("ERROR: Label filename is null.\n");
        #ifdef ANDROID
            ALOG("GTI_JNI: ERROR: Label filename is null.\n");
        #endif
 
        return;
    }

    std::ifstream labels(FileName);
    if (!labels.is_open())
    {
        printf("ERROR: Label file (%s)is not opened.\n", FileName);
        return;
    }

    while (std::getline(labels, line))
    {
        size_t pos = line.find(0x0d);
        if (pos != string::npos)
        {
            line.erase(pos, 1);
        }
        m_Labels.push_back(line);
    }
    labels.close();
}

//====================================================================
// Function name: int gtiPreLoadCoefData(char *CoefDataFileName)
// This function loads coef data from file.
//
// Input: char *CoefDataFileName - coef data file name.
// Return: int - 1: succeed, 0: failed
//====================================================================
int Classify::gtiPreloadCoefData(const char *CoefDataFcFileName)
{
    // Loading Conv Filter
    FILE *fpIn = NULL;
	size_t unused ;

    if (CoefDataFcFileName == NULL)
    {
        printf("ERROR: CoefDataFcFileName is null.\n");
        #ifdef ANDROID
            ALOG("GTI_JNI: ERROR: CoefDataFcFileName is null.\n");
        #endif
        return FALSE;
    }

    // Load FC COEF from firmware file
#ifdef WIN32
    fopen_s(&fpIn, CoefDataFcFileName, "rb");
#else
    fpIn = fopen(CoefDataFcFileName, "rb");
#endif
    if (fpIn == NULL)
    {
        printf("ERROR: Failed in open conv filter file %s.\n", CoefDataFcFileName);
        #ifdef ANDROID
            ALOG("GTI_JNI: ERROR: Failed in open conv filter file.\n");
        #endif
        return FALSE;
    }

    unused = fread_s(&m_Fc6_input_size, 4, 4, 1, fpIn);
    unused = fread_s(&m_Fc6_output_size, 4, 4, 1, fpIn);
    unused = fread_s(&m_Fc7_input_size, 4, 4, 1, fpIn);
    unused = fread_s(&m_Fc7_output_size, 4, 4, 1, fpIn);
    unused = fread_s(&m_Fc8_input_size, 4, 4, 1, fpIn);
    unused = fread_s(&m_Fc8_output_size, 4, 4, 1, fpIn);
	(void )unused;
#if 0
    printf("m_Fc6_input_size(%d), m_Fc6_output_size(%d)\n", m_Fc6_input_size, m_Fc6_output_size);
    printf("m_Fc7_input_size(%d), m_Fc7_output_size(%d)\n", m_Fc7_input_size, m_Fc7_output_size);
    printf("m_Fc8_input_size(%d), m_Fc8_output_size(%d)\n", m_Fc8_input_size, m_Fc8_output_size);

	ALOG("GTI_JNI: m_CPU_num = %d\n", m_CPU_num);
    ALOG("GTI_JNI: m_Fc6_input_size(%d), m_Fc6_output_size(%d)\n", m_Fc6_input_size, m_Fc6_output_size);
    ALOG("GTI_JNI: m_Fc7_input_size(%d), m_Fc7_output_size(%d)\n", m_Fc7_input_size, m_Fc7_output_size);

    ALOG("GTI_JNI: m_Fc8_input_size(%d), m_Fc8_output_size(%d)\n", m_Fc8_input_size, m_Fc8_output_size);
#endif

    m_Fc6input = (float *)new BYTE[m_Fc6_input_size * 4];
    memset(m_Fc6input, 0, m_Fc6_input_size * 4);

    m_Fc6coef = (float *)new BYTE[m_Fc6_input_size * m_Fc6_output_size * 4];
    if (m_Fc6input == NULL || m_Fc6coef == NULL)
    {
        printf("Failed allocat memory for fc6coef!\n");
        return FALSE;
    }
    size_t retval = fread_s((void *)m_Fc6coef, m_Fc6_input_size * m_Fc6_output_size * 4, 4, m_Fc6_input_size * m_Fc6_output_size, fpIn);

    m_Fc6bias = (float *)new BYTE[m_Fc6_output_size * 4];
    if (m_Fc6bias == NULL)
    {
        printf("Failed allocat memory for fc6bias!\n");
        return FALSE;
    }
    retval = fread_s(m_Fc6bias, m_Fc6_output_size * 4, 4, m_Fc6_output_size, fpIn);

    m_Fc7coef = (float *)new BYTE[m_Fc7_input_size * m_Fc7_output_size * 4];
    if (m_Fc7coef == NULL)
    {
        printf("Failed allocat memory for fc7coef!\n");
        return FALSE;
    }
    retval = fread_s(m_Fc7coef, m_Fc7_input_size * m_Fc7_output_size * 4, 4, m_Fc7_input_size * m_Fc7_output_size, fpIn);

    m_Fc7bias = (float *)new BYTE[m_Fc7_output_size * 4];
    if (m_Fc7bias == NULL)
    {
        printf("Failed allocat memory for fc7bias!\n");
        return FALSE;
    }
    retval = fread_s(m_Fc7bias, m_Fc7_output_size * 4, 4, m_Fc7_output_size, fpIn);

    m_Fc8coef = (float *)new BYTE[m_Fc8_input_size * m_Fc8_output_size * 4];
    if (m_Fc8coef == NULL)
    {
        printf("Failed allocat memory for fc8coef!\n");
        return FALSE;
    }
    retval = fread_s(m_Fc8coef, m_Fc8_input_size * m_Fc8_output_size * 4, 4, m_Fc8_input_size * m_Fc8_output_size, fpIn);
    if (retval != m_Fc8_input_size * m_Fc8_output_size)
    {
        printf("Read m_Fc8coef (size = %d, m_Fc8_output_size*4 = %d) error!\n", (int)retval, (int)m_Fc8_input_size * m_Fc8_output_size * 4);
    }

    m_Fc8bias = (float *)new BYTE[m_Fc8_output_size * 4];
    if (m_Fc8bias == NULL)
    {
        printf("Failed allocat memory for fc8bias!\n");
        return FALSE;
    }
    retval = fread_s(m_Fc8bias, m_Fc8_output_size * 4, 4, m_Fc8_output_size, fpIn);
    if (retval != m_Fc8_output_size)
    {
        printf("Read Fc8bias (size = %d, m_Fc8_output_size*4 = %d) error!\n", (int)retval, (int)m_Fc8_output_size*4);
    }

    fclose(fpIn);

    // Allocate FC working space
    m_Fc6_out = (float*)new BYTE[m_Fc6_output_size * sizeof(float)];
    if (m_Fc6_out == NULL)
    {
        printf("Failed allocat memory for fc6_out!\n");
        return FALSE;
    }
    m_Fc7_out = (float*)new BYTE[m_Fc7_output_size * sizeof(float)];
    if (m_Fc7_out == NULL)
    {
        printf("Failed allocat memory for fc7_out!\n");
        return FALSE;
    }
    m_Fc8_out = (float*)new BYTE[m_Fc8_output_size * sizeof(float)];
    if (m_Fc8_out == NULL)
    {
        printf("Failed allocat memory for fc8_out!\n");
        return FALSE;
    }
    m_Probs_out = (float*)new BYTE[m_Fc8_output_size * sizeof(float)];
    if (m_Probs_out == NULL)
    {
        printf("Failed allocat memory for probs_out!\n");
        return FALSE;
    }

    // Allocate layer6 accelerate FC working space
    m_Layer6_yi = new float[m_Fc6_output_size];
    if (m_Layer6_yi == NULL)
    {
        printf("Failed allocat memory for probs_out!\n");
        return FALSE;
    }

    m_Layer6_coef = new float[m_Fc6_output_size * m_Fc6_input_size];
    if (m_Layer6_coef == NULL)
    {
        printf("Failed allocat memory for probs_out!\n");
        return FALSE;
    }

    FC_THREAD_DATA threadInData;

    // Allocate layer6 accelerate FC thread working space
    int divLen = m_Fc6_input_size / m_CPU_num;
    for (int i = 0; i < m_CPU_num; i++)
    {
        float *layer6ptr = new float[m_Fc6_output_size];
        if (layer6ptr == NULL)
        {
            printf("Failed allocat memory for layer6ptr [%d]!\n", i);
            return FALSE;
        }
        m_Layer6_sum.push_back(layer6ptr);

        threadInData.firstAddress = (float *)m_Fc6input + i * divLen;
        if (i == m_CPU_num - 1)
        {
            threadInData.firstLen = m_Fc6_input_size - i * divLen;
        }
        else
        {
            threadInData.firstLen = divLen;
        }
        threadInData.secondAddress = m_Layer6_coef + i * divLen * m_Fc6_output_size;
        threadInData.secondLen = m_Fc6_output_size;
        threadInData.sumBuffer = layer6ptr;

        m_Fc6ThreadIn.push_back(threadInData);
    }

    // Set layer7 accelerate FC thread working space
    divLen = m_Fc7_output_size / m_CPU_num;
    for (int i = 0; i < m_CPU_num; i++)
    {
        threadInData.sumBuffer = m_Fc7_out + i * divLen;
        threadInData.firstAddress = m_Fc6_out;
        threadInData.firstLen = m_Fc7_input_size;
        threadInData.secondAddress = m_Fc7coef + i * divLen * m_Fc7_input_size;
        threadInData.thirdAddress = (float *)(m_Fc7bias + i * divLen);
        if (i == m_CPU_num - 1)
        {
            threadInData.secondLen = m_Fc7_output_size - i * divLen;
        }
        else
        {
            threadInData.secondLen = divLen;
        }
        m_Fc7ThreadIn.push_back(threadInData);
    }

    // Set layer8 accelerate FC thread working space
    divLen = m_Fc8_output_size / m_CPU_num;
    for (int i = 0; i < m_CPU_num; i++)
    {
        threadInData.sumBuffer = m_Fc8_out + i * divLen;
        threadInData.firstAddress = m_Fc7_out;
        threadInData.firstLen = m_Fc8_input_size;
        threadInData.secondAddress = m_Fc8coef + i * divLen * m_Fc8_input_size;
        threadInData.thirdAddress = (float *)(m_Fc8bias + i * divLen);
        if (i == m_CPU_num - 1)
        {
            threadInData.secondLen = m_Fc8_output_size - i * divLen;
        }
        else
        {
            threadInData.secondLen = divLen;
        }
        m_Fc8ThreadIn.push_back(threadInData);
    }

    // transval coef matrix to accelerate process:
    for (DWORD i = 0; i < m_Fc6_input_size; i++)
    {
        for (DWORD j = 0; j < m_Fc6_output_size; j++)
        {
            int m = i * m_Fc6_output_size + j;
            int n = j * m_Fc6_input_size + i;
            *(m_Layer6_coef + m) = *(m_Fc6coef + n);
        }
    }

    return TRUE;
}

//====================================================================
// Function name: void *gtiFc6ThreadFunc(void* arg)
// This function is for thread of FC layer 6.
//
// Input: void *arg - input data with struct pFC_THREAD_DATA.
// Return: void * - a pointer to input arg.
//====================================================================
void *gtiFc6ThreadFunc(void* arg)
{
    pFC_THREAD_DATA inData = (pFC_THREAD_DATA)arg;
    int input_nums = inData->firstLen;
    float *pInput = inData->firstAddress;
    int output_nums = inData->secondLen;
    float *pCoef = inData->secondAddress;
    float *pSum = inData->sumBuffer;
    memset(pSum, 0, output_nums * sizeof(float));
    for (int i = 0; i < input_nums; i++)
    {
        float x = *(pInput + i);
        if (x == 0)
        {
            pCoef += output_nums;
            continue;
        }
        else
        {
            for (int j = 0; j < output_nums; j++)
            {
                pSum[j] += *(pCoef++) * x;
            }
        }
    }
    return arg;
}

//====================================================================
// Function name: void *gtiFc7ThreadFunc(void* arg)
// This function is for thread of FC layer 7.
//
// Input: void *arg - input data with struct pFC_THREAD_DATA.
// Return: void * - a pointer to input arg.
//====================================================================
void *gtiFc7ThreadFunc(void* arg)
{
    pFC_THREAD_DATA inData = (pFC_THREAD_DATA)arg;
    int input_nums = inData->firstLen;
    float *pInput = inData->firstAddress;
    int output_nums = inData->secondLen;
    float *pCoef = inData->secondAddress;
    float *pBias = inData->thirdAddress;
    float *pSum = inData->sumBuffer;
    for (int i = 0; i < output_nums; i++)
    {
        double yi = 0;
        for (int j = 0; j < input_nums; j++)
        {
            float aj = *(pCoef + j);
            float xj = *(pInput + j);
            yi += aj * xj;
        }
        pCoef += input_nums;
        float bi = *(pBias + i);
        yi += bi;

        *(pSum + i) = (yi < 0) ? 0 : (float)yi;
    }
    return arg;
}

//====================================================================
// Function name: void *gtiFc8ThreadFunc(void* arg)
// This function is for thread of FC layer 8.
//
// Input: void *arg - input data with struct pFC_THREAD_DATA.
// Return: void * - a pointer to input arg.
//====================================================================
void *gtiFc8ThreadFunc(void* arg)
{
    pFC_THREAD_DATA inData = (pFC_THREAD_DATA)arg;
    int input_nums = inData->firstLen;
    float *pInput = inData->firstAddress;
    int output_nums = inData->secondLen;
    float *pCoef = inData->secondAddress;
    float *pBias = inData->thirdAddress;
    float *pSum = inData->sumBuffer;
    for (int i = 0; i < output_nums; i++)
    {
        double yi = 0;
        for (int j = 0; j < input_nums; j++)
        {
            float aj = *(pCoef + j);
            float xj = *(pInput + j);
            yi += aj * xj;
        }
        pCoef += input_nums;
        float bi = *(pBias + i);
        yi += bi;

        *(pSum + i) = (float)yi;
    }
    return arg;
}

//====================================================================
// Function name: float *gtiAccelerateFC(unsigned int *FcOutLen)
// This function accelerates FC.
//
// Input: uint *FcOutLen - address to FC output length.
// Return: float * - accelerated FC result address.
//====================================================================
float* Classify::gtiAccelerateFC(unsigned int *FcOutLen)
{
    vector<pthread_t> tid;
    //cpu_set_t cpuset;

    //CPU_ZERO(&cpuset);
    //for (int i = 0; i < 8; i++)
    //    CPU_SET(i, &cpuset);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // FC process, FCMode: 
    //		FCCOEF: filter from firmware file
    //		Accel: Wenhan's FC Accelarating algorithm with filter from caffe-model
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //vector<float> output;
    //const float* begin = NULL;
    //const float* end = NULL;

    if (!m_CoefDataLoad)
    {
        return NULL;
    }

    //------ fc6 layer -----
    // fc6 data:
    //float* fc6_input = (float*)fc6input;

    //if (m_Fc6_out == NULL)
    //{
    //    printf("Failed to allocat memory for fc6_out!\n");
    //    exit(-1);
    //}

    //int fc6_input_nums = m_Fc6_input_size;
    //int fc6_output_nums = m_Fc6_output_size;

    // fc6 parameters:
    //const float* fc6p1 = m_Fc6bias;
    //float *yi = m_Layer6_yi;
    //if (yi == NULL)
    //{
    //    printf("Failed to allocat memory for fc6 parameters!\n");
    //    exit(-1);
    //}
    memset(m_Layer6_yi, 0, sizeof(float) * m_Fc6_output_size);
    //for (int i = 0; i < fc6_output_nums; i++)
    //{
    //    *(yi + i) = 0.0;
    //}

    // transval coef matrix to accelerate process:
    //float *coef = m_Layer6_coef;
    //if (coef == NULL)
    //{
    //    printf("Failed to allocat memory for fc6 coef!\n");
    //    exit(-1);
    //}

    // if data ne zero, calculate vector multiply
    //float *pt = coef;
    /*
    // Layer 6 FC calculation
    //
    for (int i = 0; i < fc6_input_nums; i++)
    {
        float x = *(fc6_input + i);
        if (x == 0)
        {
            pt += fc6_output_nums;
            continue;
        }
        else
        {
            for (int j = 0; j < fc6_output_nums; j++)
            {
                yi[j] += *(pt++) * x;
            }
        }
    }
    //
    // Following code is multi-thread for Fc 6
    */
    for (unsigned int i = 0; i < m_Fc6ThreadIn.size(); i++)
    {
        pthread_t tid6;

        // Create thread
        pthread_create(&tid6, NULL, gtiFc6ThreadFunc, (void *)&m_Fc6ThreadIn[i]);
        //pthread_setaffinity_np(tid[i], sizeof(cpu_set_t), &cpuset);
        tid.push_back(tid6);
    }
    for (unsigned int i = 0; i < m_Fc6ThreadIn.size(); i++)
    {
        pthread_join(tid[i], NULL);
        for (unsigned int j = 0; j < m_Fc6_output_size; j++)
        {
            m_Layer6_yi[j] += m_Fc6ThreadIn[i].sumBuffer[j];
        }
    }
    tid.clear();

    // ReLU
    for (unsigned int i = 0; i < m_Fc6_output_size; i++)
    {
        float bi = *(m_Fc6bias + i);
        float y = (float)(m_Layer6_yi[i] + bi);

        *(m_Fc6_out + i) = (y < 0) ? 0 : y;
    }
    //	layer fc6 end

    //------ fc7 layer ------
    //if (m_Fc7_out == NULL)
    //{
    //    printf("Failed allocat memory for fc7_out!\n");
    //    exit(-1);
    //}
    // fc7 parameters:
    //const float* fc7p0 = m_Fc7coef;
    //const float* fc7p1 = m_Fc7bias;

    //int input_size = m_Fc7_input_size;
    //int output_size = m_Fc7_output_size;
    //float *fc7p0_ptr = (float *)fc7p0;
    /*
    // Layer 7 FC calculation
    //
    for (int i = 0; i < output_size; i++)
    {
        double yi = 0;
        for (int j = 0; j < input_size; j++)
        {
            float aj = *(fc7p0_ptr + j);
            float xj = *(m_Fc6_out + j);
            yi += aj * xj;
        }
        fc7p0_ptr += input_size;
        float bi = *(fc7p1 + i);
        yi += bi;

        *(m_Fc7_out + i) = (yi < 0) ? 0 : (float)yi;
    }
    //
    // Following code is multi-thread for Fc 7
    */
    for (unsigned int i = 0; i < m_Fc7ThreadIn.size(); i++)
    {
        pthread_t tid7;

        // Create thread
        pthread_create(&tid7, NULL, gtiFc7ThreadFunc, (void *)&m_Fc7ThreadIn[i]);
        //pthread_setaffinity_np(tid[i], sizeof(cpu_set_t), &cpuset);
        tid.push_back(tid7);
    }
    for (unsigned int i = 0; i < m_Fc7ThreadIn.size(); i++)
    {
        pthread_join(tid[i], NULL);
    }
    tid.clear();
    //	layer fc7 end

    //------ fc8 layer ------
    // fc8 data:
    //if (m_Fc8_out == NULL)
    //{
    //    printf("Failed allocat memory for fc8_out!\n");
    //    exit(-1);
    //}

    //input_size = m_Fc8_input_size;
    //output_size = m_Fc8_output_size;

    // fc8 parameters:
    //const float* fc8p0 = m_Fc8coef;
    //const float* fc8p1 = m_Fc8bias;

    //float *fc8p0_ptr = (float *)fc8p0;
    /*
    // Layer 8 FC calculation
    //
    for (int i = 0; i < output_size; i++)
    {
        double yi = 0;
        for (int j = 0; j < input_size; j++)
        {
            float aj = *(fc8p0_ptr + j);
            float xj = *(m_Fc7_out + j);
            yi += aj * xj;
        }
        fc8p0_ptr += input_size;
        // float bi = *(fc8p1 + i);
        yi += *(fc8p1 + i);

        *(m_Fc8_out + i) = (float)yi;	// (yi < 0) ? 0 : yi;
    }
    //
    // Following code is multi-thread for Fc 8
    */
    for (unsigned int i = 0; i < m_Fc8ThreadIn.size(); i++)
    {
        pthread_t tid8;

        // Create thread
        pthread_create(&tid8, NULL, gtiFc8ThreadFunc, (void *)&m_Fc8ThreadIn[i]);
        //pthread_setaffinity_np(tid[i], sizeof(cpu_set_t), &cpuset);
        tid.push_back(tid8);
    }
    for (unsigned int i = 0; i < m_Fc8ThreadIn.size(); i++)
    {
        pthread_join(tid[i], NULL);
    }
    tid.clear();
    //	layer fc8 end

    //------ softmax layer ------
    //if (m_Probs_out == NULL)
    //{
    //    printf("Failed allocat memory for probs_out!\n");
    //    exit(-1);
    //}
    int channels = m_Fc8_output_size;

    //1. find max value:
    float max = 0;
    for (int i = 0; i < channels; i++)
    {
        float x = *(m_Fc8_out + i);
        if (max < x)
        {
            max = x;
        }
    }
    //2. exp and sum: max = 0;
    double sum = 0;
    for (int i = 0; i < channels; i++)
    {
        double x = exp(*(m_Fc8_out + i) - max);
        *(m_Probs_out + i) = (float)x;
        sum += x;
    }
    //3. normalize:
    sum = 1.0 / sum;
    for (int i = 0; i < channels; i++)
    {
        double x = *(m_Probs_out + i);
        *(m_Probs_out + i) = (float)(x * sum);
    }
    //	layer prob end

    *FcOutLen = channels;
    return m_Probs_out;

    /* Copy the output layer to a std::vector */
    //begin = m_Probs_out;
    //end = begin + channels;

    //output = vector<float>(begin, end);
    //return output;
}

//====================================================================
// Function name: static bool gtiPairCompare(const std::pair<float, int>& lhs, const std::pair<float, int>& rhs)
// This function compares two values for pair lhs and rhs.
//
// Input: std::pair<float, int>& lhs - input pair.
//        std::pair<float, int>& rhs - input pair.
// Return: 1 - if lhr.first > rhs.first.
//         0 - if lhr.first <= rhs.first.
//====================================================================
static bool gtiPairCompare(const std::pair<float, int>& lhs, const std::pair<float, int>& rhs)
{
    return lhs.first > rhs.first;
}

//====================================================================
// Function name: vector<int> gtiArgmax(const vector<float>& v, int N)
// This function gets the indices of the top N values of vector v.
//
// Input: const vector<float>& v - input vector.
//        int N - the count of output values.
// Return: vector<int> - result vector data.
//====================================================================
//vector<int> Classify::gtiArgmax(const vector<float>& v, int N)
vector<int> Classify::gtiArgmax(float* FcOut, unsigned int FcLen, int N)
{
    vector<std::pair<float, int> > pairs;
    for (unsigned int i = 0; i < FcLen; ++i)
    {
        pairs.push_back(std::make_pair(FcOut[i], static_cast<int>(i)));
    }

    std::partial_sort(pairs.begin(), pairs.begin() + N, pairs.end(), gtiPairCompare);

    vector<int> result;
    for (int i = 0; i < N; ++i)
    {
        result.push_back(pairs[i].second);
    }

    return result;
}

char *Classify::GtiGetPredicationString(int Index)
{
    m_PredText = to_string(m_Predications[Index].second) + string(" - ") + m_Predications[Index].first;
    return (char *)m_PredText.c_str();
}

int Classify::GtiGetPredicationSize()
{
    return (int)m_Predications.size();
}

float *Classify::GtiGetFeatureVector()
{
    return m_Fc7_out;
}

DWORD Classify::GtiGetFeatureVectorLength()
{
    return m_Fc7_output_size;
}


//
// Wrapper for python
//
Classify *GtiClassifyCreate(const char *coefName, const char *labelName)
{
    return new Classify((char *)coefName, (char *)labelName);
}

void GtiClassifyRelease(Classify *gtiClassify)
{
    if (gtiClassify)
    {
        delete gtiClassify;
    }
}

int GtiClassifyFC(Classify *gtiClassify, float *inputData, int count)
{
    return gtiClassify->GtiClassify(inputData, count);
}

char *GetPredicationString(Classify *gtiClassify, int index)
{
    return gtiClassify->GtiGetPredicationString(index);
}

int GetPredicationSize(Classify *gtiClassify)
{
    return gtiClassify->GtiGetPredicationSize();
}

float *GetFeatureVector(Classify *gtiClassify){
    return gtiClassify->GtiGetFeatureVector();
}

DWORD GetFeatureVectorLength(Classify *gtiClassify){
    return gtiClassify->GtiGetFeatureVectorLength();
}
