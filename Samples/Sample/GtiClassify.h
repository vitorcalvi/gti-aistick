/***********************************************************************
*
* Copyright (c) 2017-2019 Gyrfalcon Technology Inc. All rights reserved.
* See LICENSE file in the project root for full license information.
*
************************************************************************/

/***************************************************************
 * GtiClassify.h
 * GtiClassify public header file.
 *
 *                Copyrights, Gyrfalcon technology Inc. 2017
 ***************************************************************/
#pragma once

class Classify;

extern "C"
{
    Classify *GtiClassifyCreate(const char *coefName, const char *labelName);
    void GtiClassifyRelease(Classify *gtiClassify);
    int GtiClassifyFC(Classify *gtiClassify, float *inputData, int count);
    char *GetPredicationString(Classify *gtiClassify, int index);
    int GetPredicationSize(Classify *gtiClassify);
    float *GetFeatureVector(Classify *gtiClassify);
    DWORD GetFeatureVectorLength(Classify *gtiClassify);
}


