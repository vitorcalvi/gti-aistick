#***********************************************************************
#
# Copyright (c) 2017-2019 Gyrfalcon Technology Inc. All rights reserved.
# See LICENSE file in the project root for full license information.
#
#************************************************************************

from ctypes import *
import os
import platform
import warnings
import functools

CPU_ARCH=os.uname()[4]
OS_TYPE=platform.system()
ARCH_SIZE=sizeof(c_voidp)

if(OS_TYPE == "Windows"):
    raise Exception('Windows is not supported')
else:
    LIBNAME = "libGtiClassify.so"

    GTILIB0 = os.path.split( __file__ )[0] + "/" + LIBNAME
    GTILIB1 = os.path.join('/', 'usr', 'local', 'lib', LIBNAME).encode('ascii')
    GTILIB2 = os.path.join('..', '..', 'Lib', OS_TYPE, CPU_ARCH, LIBNAME).encode('ascii')

    try:
        libgc = CDLL(GTILIB0)
    except Exception as e:
        print( "Could not find %s in the current package directory" %LIBNAME )
        print( type(e), e.args )
        print( "Try to find %s in the system path" %LIBNAME)
        try:
            libgc = CDLL(GTILIB1)
        except Exception as e:
            print( "Could not find %s in the system library path" %LIBNAME )
            print( type(e), e.args )
            print( "Try to find %s in the current SDK package" %LIBNAME)
            try:
                libgc = CDLL(GTILIB2)
            except Exception as e2:
                print( "Could not find %s in the current SDK package" %LIBNAME )
                print( "Please build the %s with \"make -C ../../Samples/Sample classifylib\"" %LIBNAME )
                print( "and copy to package with \"cp ../../Samples/Sample/%s ../../Python/Lib/gtiClassify/gtiClassify/\"" %LIBNAME )
                print( "then pip re-install gtiClassify package." ) 
                print( type(e2), e2.args )
                print( "Stop here" )
                raise
            else:
                print( "Using %s" %GTILIB2 )
        else:
            print( "Using %s" %GTILIB1 )
    else:
        print( "Using %s" %GTILIB0 )


if(ARCH_SIZE == 4):
    print( "Running on a 32-bit machine" )
elif(ARCH_SIZE == 8):
    print( "Running on a 64-bit machine" )
else:
    print( "Cannot determine 32-bit or 64-bit machine" )
    raise


def Deprecated(func):
    @functools.wraps(func)
    def wrapper_with_warning(*args, **kwargs):
        warnings.warn("Deprecated function: %s"%func.func_name, DeprecationWarning)
        return func(*args, **kwargs)
    return wrapper_with_warning

def Future(func):
    @functools.wraps(func)
    def wrapper_with_warning(*args, **kwargs):
        warnings.warn("Future function: %s, undefined yet"%func.func_name, FutureWarning)
        return func(*args, **kwargs)
    return wrapper_with_warning

class Classify(object):
    @Deprecated
    def __init__(self, coefFile, labelFile):
        if(ARCH_SIZE == 4):
            libgc.GtiClassifyCreate.restype = c_ulong
        else:
            libgc.GtiClassifyCreate.restype = c_ulonglong
        self.obj = libgc.GtiClassifyCreate(coefFile, labelFile)

    @Deprecated
    def gClassifyRelease(self):
        if(ARCH_SIZE == 4):
            libgc.GtiClassifyRelease.argtypes = (c_ulong,)
        else:
            libgc.GtiClassifyRelease.argtypes = (c_ulonglong,)
        libgc.GtiClassifyRelease(self.obj)

    @Deprecated
    def gClassify(self, inputData, count):
        ''' This Classify function assumes the input data is a pointer an array of byte '''
        if(ARCH_SIZE == 4):
            libgc.GtiClassifyFC.argtypes = (c_ulong, c_char_p, c_int)
        else:
            libgc.GtiClassifyFC.argtypes = (c_ulonglong, c_char_p, c_int)
        return libgc.GtiClassifyFC(self.obj, inputData, count)

    @Deprecated
    def gClassifyFloatIn(self, inputData, count):
        ''' This Classify function assumes the input data is a pointer an array of float '''
        if(ARCH_SIZE == 4):
            libgc.GtiClassifyFC.argtypes = (c_ulong, POINTER(c_float), c_int)
        else:
            libgc.GtiClassifyFC.argtypes = (c_ulonglong, POINTER(c_float), c_int)
        return libgc.GtiClassifyFC(self.obj, inputData, count)

    @Deprecated
    def gGetPredicationString(self, index):
        if(ARCH_SIZE == 4):
            libgc.GetPredicationString.argtypes = (c_ulong, c_int)
        else:
            libgc.GetPredicationString.argtypes = (c_ulonglong, c_int)
        libgc.GetPredicationString.restype = c_char_p
        return libgc.GetPredicationString(self.obj, index)

    @Deprecated
    def gGetPredicationSize(self):
        if(ARCH_SIZE == 4):
            libgc.GetPredicationSize.argtypes = (c_ulong,)
        else:
            libgc.GetPredicationSize.argtypes = (c_ulonglong,)
        return libgc.GetPredicationSize(self.obj)

