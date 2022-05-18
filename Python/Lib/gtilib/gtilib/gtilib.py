#***********************************************************************
#
# Copyright (c) 2017-2019 Gyrfalcon Technology Inc. All rights reserved.
# See LICENSE file in the project root for full license information.
#
#************************************************************************

from ctypes import *
import os
import sys
import platform
import warnings
import functools
from enum import Enum
import numpy as np
import struct


CPU_ARCH=os.uname()[4]
OS_TYPE=platform.system()
ARCH_SIZE=sizeof(c_voidp)
if(OS_TYPE == "Windows"):
    LIBNAME = "libGTILibrary.dll"
    GTISDK_PATH = os.environ['GTISDKPATH']
    GTILIB = os.path.join( GTISDK_PATH, 'Lib', OS_TYPE, CPU_ARCH, LIBNAME).encode('ascii')

    try:
        gtiSdkLib = CDLL(GTILIB)
    except Exception as e:
        print( "Oops! Error in opening %s, received exception" %GTILIB )
        print( type(e), e.args )
        print( "Trying Lib directory in SDK package" )
        raise
else:
    LIBNAME = "libGTILibrary.so"

    GTILIB0 = os.path.split( __file__ )[0] + "/" + LIBNAME
    GTILIB1 = os.path.join('/', 'usr', 'local', 'lib', LIBNAME).encode('ascii')
    GTILIB2 = os.path.join('..', '..', 'Lib', OS_TYPE, CPU_ARCH, LIBNAME).encode('ascii')

    try:
        gtiSdkLib = CDLL(GTILIB0)
    except Exception as e:
        print( "Could not find %s in the current package directory" %LIBNAME )
        print( type(e), e.args )
        print( "Try to find %s in the system path" %LIBNAME)
        try:
            gtiSdkLib = CDLL(GTILIB1)
        except Exception as e:
            print( "Could not find %s in the system library path" %LIBNAME )
            print( type(e), e.args )
            print( "Try to find %s in the current SDK package" %LIBNAME)
            try:
                gtiSdkLib = CDLL(GTILIB2)
            except Exception as e2:
                print( "Could not find %s in the current SDK package" %LIBNAME )
                print( "Please contact your package provider for %s" %LIBNAME )
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


class GTI_DEVICE_STATUS(Enum): 
    GTI_DEVICE_STATUS_ERROR = 0
    GTI_DEVICE_STATUS_ADDED = 1
    GTI_DEVICE_STATUS_REMOVED = 2
    GTI_DEVICE_STATUS_IDLE = 3
    GTI_DEVICE_STATUS_LOCKED = 4
    GTI_DEVICE_STATUS_RUNNING = 5

class GTI_DEVICE_TYPE(Enum):
    GTI_DEVICE_TYPE_ALL = 0
    GTI_DEVICE_USB_FTDI = 1
    GTI_DEVICE_USB_EUSB = 2
    GTI_DEVICE_PCIE = 3
    GTI_DEVICE_TYPE_BUT = 4

class GTI_CHIP_MODE(Enum):
    FC_MODE = 0
    LEARN_MODE = 1
    SINGLE_MODE = 2
    SUBLAST_MODE = 3
    LASTMAJOR_MODE = 4
    LAST7x7OUT_MODE = 5
    GTI_CHIP_MODE_BUT = 6

class TENSOR_FORMAT(Enum):
    TENSOR_FORMAT_BINARY = 0
    TENSOR_FORMAT_TEXT = 1
    TENSOR_FORMAT_JSON = 2
    TENSOR_FORMAT_BUT = 3

def GtiGetSDKVersion():
    gtiSdkLib.GtiGetSDKVersion.argtypes = ()
    gtiSdkLib.GtiGetSDKVersion.restype = c_char_p
    return gtiSdkLib.GtiGetSDKVersion()

def GtiComposeModelFile(jsonFile, modelFile):
    gtiSdkLib.GtiComposeModelFile.argtypes = (c_char_p, c_char_p)
    return gtiSdkLib.GtiComposeModelFile(jsonFile.encode('ascii'), modelFile.encode('ascii'))

def GtiDecomposeModelFile(modelFile):
    gtiSdkLib.GtiDecomposeModelFile.argtypes = (c_char_p,)
    return gtiSdkLib.GtiDecomposeModelFile(modelFile.encode('ascii'))


class GtiTensor(Structure):
    pass

GtiTensor._fields_ = [ ("width", c_int),
                 ("height", c_int),
                 ("depth", c_int),
                 ("stride", c_int),
                 ("buffer", POINTER(c_char)),
                 ("size", c_int),
                 ("format", c_int), # TENSOR_FORMAT
                 ("tag", POINTER(c_char)),
                 ("private2", POINTER(c_char)),
                 ("private3", POINTER(c_char)) ]


class GtiContext(object):
    ''' Collection of devices '''
    def __init__(self):
        gtiSdkLib.GtiGetContext.argtypes = ()
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiGetContext.restype = c_ulong
        else:
            gtiSdkLib.GtiGetContext.restype = c_ulonglong
        self.obj = gtiSdkLib.GtiGetContext()

    def GtiGetDevice(self, devicePlatformName):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiGetDevice.argtypes = (c_ulong, c_char_p)
        else:
            gtiSdkLib.GtiGetDevice.argtypes = (c_ulonglong, c_char_p)
        return gtiSdkLib.GtiGetDevice(self.obj, devicePlatformName.encode('ascii'))

    def GtiGetAvailableDevice(self, deviceType=GTI_DEVICE_TYPE.GTI_DEVICE_USB_EUSB):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiGetAvailableDevice.argtypes = (c_ulong, c_int)
        else:
            gtiSdkLib.GtiGetAvailableDevice.argtypes = (c_ulonglong, c_int)
        return gtiSdkLib.GtiGetAvailableDevice(self.obj, deviceType)


class GtiModel(object):
    ''' A model contains all layers, represents a network '''
    def __init__(self, modelName):
        gtiSdkLib.GtiCreateModel.argtypes=(c_char_p,)
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiCreateModel.restype = c_ulong
        else:
            gtiSdkLib.GtiCreateModel.restype = c_ulonglong
        self.obj = gtiSdkLib.GtiCreateModel(modelName.encode('ascii'))

    def __del__(self):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiDestroyModel.argtypes=(c_ulong,)
        else:
            gtiSdkLib.GtiDestroyModel.argtypes=(c_ulonglong,)
        gtiSdkLib.GtiDestroyModel.restype = c_int
        gtiSdkLib.GtiDestroyModel(self.obj)

    def GtiImageEvaluate(self, image, height, width, depth):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiImageEvaluate.argtypes = (c_ulong, c_char_p, c_int, c_int, c_int)
        else:
            gtiSdkLib.GtiImageEvaluate.argtypes = (c_ulonglong, c_char_p, c_int, c_int, c_int)
        gtiSdkLib.GtiImageEvaluate.restype = c_char_p
        return gtiSdkLib.GtiImageEvaluate(self.obj, image, height, width, depth)

    def GtiImageEvaluate_oneFrame(self, image, w, h, d,outLen):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiImageEvaluate.argtypes=(c_ulong, c_char_p, c_int, c_int, c_int)
        else:
            gtiSdkLib.GtiImageEvaluate.argtypes=(c_ulonglong, c_char_p, c_int, c_int, c_int)
        gtiSdkLib.GtiImageEvaluate.restype = POINTER(c_float*outLen)
        out = gtiSdkLib.GtiImageEvaluate(self.obj, image, w, h, d)
        return [int(i) for i in out.contents]

    def GtiCreateModel(self, modelFile):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiCreateModel.argtypes = (c_ulong, c_char_p)
            gtiSdkLib.GtiCreateModel.restype = c_ulong
        else:
            gtiSdkLib.GtiCreateModel.argtypes = (c_ulonglong, c_char_p)
            gtiSdkLib.GtiCreateModel.restype = c_ulonglong
        return gtiSdkLib.GtiCreateModel(self.obj, modelFile)

    def GtiCreateModelFromBuffer(self, modelBuffer, modelSize):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiCreateModelFromBuffer.argtypes = (c_ulong, c_char_p, c_int)
            gtiSdkLib.GtiCreateModelFromBuffer.restype = c_ulong
        else:
            gtiSdkLib.GtiCreateModelFromBuffer.argtypes = (c_ulonglong, c_char_p, c_int)
            gtiSdkLib.GtiCreateModelFromBuffer.restype = c_ulonglong
        return gtiSdkLib.GtiCreateModelFromBuffer(self.obj, modelBuffer, modelSize)

    def GtiDestroyModel(self):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiDestroyModel.argtypes=(c_ulong,)
        else:
            gtiSdkLib.GtiDestroyModel.argtypes=(c_ulonglong,)
        gtiSdkLib.GtiDestroyModel.restype = c_int
        return gtiSdkLib.GtiDestroyModel(self.obj)

    def GtiEvaluateFloatOut(self, image, width, height, depth, stride=1, tensorformat=0):
        tin = GtiTensor()
        tin.width = width
        tin.height = height
        tin.depth = depth
        tin.stride = stride
        tin.size = width*height*depth
        tin.format = tensorformat
        c_buf = create_string_buffer(image)
        tin.buffer = cast(c_buf, POINTER(c_char))
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiEvaluate.argtypes=(c_ulong, POINTER(GtiTensor) )
        else:
            gtiSdkLib.GtiEvaluate.argtypes=(c_ulonglong, POINTER(GtiTensor) )
        gtiSdkLib.GtiEvaluate.restype = POINTER(GtiTensor)
        out_ref = gtiSdkLib.GtiEvaluate(self.obj, pointer(tin) )
        tout = out_ref.contents
        buffer_out_count = tout.width*tout.height*tout.depth
        buffer_out = cast( tout.buffer, POINTER(c_float * buffer_out_count) )
        return buffer_out.contents, tout.size, tout.width, tout.height, tout.depth

    def GtiEvaluate(self, image, w, h, d):
        # -----------input----------
        #      image:     numpary array, numpy.int32
        #          w:     int, weight of image
        #          h:     int, height of image
        #          d:     int, depth of image
        # ---------- -ouput----------
        # buffer_out:     pointer of output
        #   size_out:     int, size of buffer
        #      w_out:     int, weight of image
        #      h_out:     int, height of image
        #      d_out:     int, depth of image
        input_buffer = bytes()
        input_buffer = struct.pack('iiii',w,h,d,0)
        img_buf = image.ctypes.data
        input_buffer +=struct.pack('q',img_buf)

        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiEvaluate.argtypes=(c_ulong, c_char_p)
        else:
            gtiSdkLib.GtiEvaluate.argtypes=(c_ulonglong, c_char_p)
        out_ref = gtiSdkLib.GtiEvaluate(self.obj, input_buffer)

        w_out = cast(out_ref, POINTER(c_int)).contents.value
        h_out = cast(out_ref + 4, POINTER(c_int)).contents.value
        d_out = cast(out_ref + 8, POINTER(c_int)).contents.value
        s_out = cast(out_ref + 12, POINTER(c_int)).contents.value
        buffer_out = cast(out_ref + 16, POINTER(c_int)).contents.value
        size_out = cast(out_ref + 24, POINTER(c_int)).contents.value

        INTP = POINTER(c_float)
        res = cast(buffer_out, INTP)
        return res,size_out,w_out,h_out,d_out

    def ImageEvaluate(self, image, height, width, depth):
        return self.GtiImageEvaluate(image, height, width, depth)


class GtiDevice(object):
    ''' A collection of device operations '''
    def __init__(self, DeviceType, FilterFileName='', ConfigFileName=''):
        ''' Due to the deprecation of DeviceCreate, need to allow this function to be by passed '''
        gtiSdkLib.GtiDeviceCreate.argtypes=(c_int, c_char_p, c_char_p)
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiDeviceCreate.restype = c_ulong
        else:
            gtiSdkLib.GtiDeviceCreate.restype = c_ulonglong
        self.obj = gtiSdkLib.GtiDeviceCreate(DeviceType, FilterFileName.encode('ascii'), ConfigFileName.encode('ascii'))

    @Deprecated
    def OpenDevice(self, DeviceName):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiOpenDevice.argtypes = (c_ulong, c_char_p)
        else:
            gtiSdkLib.GtiOpenDevice.argtypes = (c_ulonglong, c_char_p)
        gtiSdkLib.GtiOpenDevice.restype = c_int
        return gtiSdkLib.GtiOpenDevice(self.obj, DeviceName.encode('ascii'))

    @Deprecated
    def CloseDevice(self):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiCloseDevice.argtypes = (c_ulong,)
        else:
            gtiSdkLib.GtiCloseDevice.argtypes = (c_ulonglong,)
        return gtiSdkLib.GtiCloseDevice(self.obj)

    @Deprecated
    def GtiDeviceCreate(self, DeviceType, FilterFileName='', ConfigFileName=''):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiDeviceCreate.argtypes = (c_ulong, c_int, c_char_p, c_char_p)
            gtiSdkLib.GtiDeviceCreate.restype = c_ulong
        else:
            gtiSdkLib.GtiDeviceCreate.argtypes = (c_ulonglong, c_int, c_char_p, c_char_p)
            gtiSdkLib.GtiDeviceCreate.restype = c_ulonglong
        return gtiSdkLib.GtiDeviceCreate(self.obj, DeviceType, FilterFileName.encode('ascii'), ConfigFileName.encode('ascii'))

    @Deprecated
    def GtiDeviceRelease(self):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiDeviceRelease.argtypes = (c_ulong,)
        else:
            gtiSdkLib.GtiDeviceRelease.argtypes = (c_ulonglong,)
        return gtiSdkLib.GtiDeviceRelease(self.obj)

    @Deprecated
    def Initialization(self):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiInitialization.argtypes = (c_ulong,)
        else:
            gtiSdkLib.GtiInitialization.argtypes = (c_ulonglong,)
        return gtiSdkLib.GtiInitialization(self.obj)

    @Deprecated
    def SelectNetwork(self, NetworkId):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiSelectNetwork.argtypes = (c_ulong, c_int)
        else:
            gtiSdkLib.GtiSelectNetwork.argtypes = (c_ulonglong, c_int)
        return gtiSdkLib.GtiSelectNetwork(self.obj, NetworkId)

    @Deprecated
    def SendImage(self, Image224Buffer, BufferLen):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiSendImage.argtypes = (c_ulong, c_char_p, c_int)
        else:
            gtiSdkLib.GtiSendImage.argtypes = (c_ulonglong, c_char_p, c_int)
        return gtiSdkLib.GtiSendImage(self.obj, Image224Buffer, BufferLen)

    @Deprecated
    def SendImageFloat(self, Image224Buffer, BufferLen):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiSendImageFloat.argtypes = (c_ulong, c_char_p, c_int)
        else:
            gtiSdkLib.GtiSendImageFloat.argtypes = (c_ulonglong, c_char_p, c_int)
        return gtiSdkLib.GtiSendImageFloat(self.obj, Image224Buffer, BufferLen)

    @Deprecated
    def SendTiledImage(self, Image224Buffer, BufferLen):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiSendTiledImage.argtypes = (c_ulong, c_char_p, c_int)
        else:
            gtiSdkLib.GtiSendTiledImage.argtypes = (c_ulonglong, c_char_p, c_int)
        return gtiSdkLib.GtiSendTiledImage(self.obj, Image224Buffer, BufferLen)

    @Deprecated
    def GetOutputData(self, OutputBuffer, BufferLen):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiGetOutputData.argtypes = (c_ulong, c_char_p, c_int)
        else:
            gtiSdkLib.GtiGetOutputData.argtypes = (c_ulonglong, c_char_p, c_int)
        return gtiSdkLib.GtiGetOutputData(self.obj, OutputBuffer, BufferLen)

    @Deprecated
    def GetOutputDataFloat(self, OutputBuffer, BufferLen):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiGetOutputDataFloat.argtypes = (c_ulong, c_char_p, c_int)
        else:
            gtiSdkLib.GtiGetOutputDataFloat.argtypes = (c_ulonglong, c_char_p, c_int)
        return gtiSdkLib.GtiGetOutputDataFloat(self.obj, OutputBuffer, BufferLen)

    def GtiHandleOneFrame(self, inputBuffer, inputLen, outputBuffer, outLen):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiHandleOneFrame.argtypes = (c_ulong, c_char_p, c_int, c_char_p, c_int)
        else:
            gtiSdkLib.GtiHandleOneFrame.argtypes = (c_ulonglong, c_char_p, c_int, c_char_p, c_int)
        return gtiSdkLib.GtiHandleOneFrame(self.obj, inputBuffer, inputLen, outputBuffer, outLen)

    def HandleOneFrameFloat(self, inputBuffer, inputLen, outputBuffer, outLen):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiHandleOneFrameFloat.argtypes = (c_ulong, c_char_p, c_int, c_char_p, c_int)
        else:
            gtiSdkLib.GtiHandleOneFrameFloat.argtypes = (c_ulonglong, c_char_p, c_int, c_char_p, c_int)
        return gtiSdkLib.GtiHandleOneFrameFloat(self.obj, inputBuffer, inputLen, outputBuffer, outLen)

    def HandleOneFrameLt(self, inBuff, inLen, outBuff, outLen):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiHandleOneFrameFloat.argtypes = (c_ulong, c_char_p, c_int, c_char_p, c_int)
        else:
            gtiSdkLib.GtiHandleOneFrameFloat.argtypes = (c_ulonglong, c_char_p, c_int, c_char_p, c_int)
        ret = gtiSdkLib.GtiHandleOneFrameFloat(self.obj, inBuff, inLen, outBuff, outLen)
        outList = np.frombuffer(outBuff, dtype='float32', count=outLen, offset=0)
        return ret, outList

    def GetOutputLength(self):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiGetOutputLength.argtypes = (c_ulong,)
        else:
            gtiSdkLib.GtiGetOutputLength.argtypes = (c_ulonglong,)
        return gtiSdkLib.GtiGetOutputLength(self.obj)

    @Deprecated
    def OpenDeviceAndInit(self, deviceName):
        # Open device.
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiOpenDevice.argtypes = (c_ulong, c_char_p)
        else:
            gtiSdkLib.GtiOpenDevice.argtypes = (c_ulonglong, c_char_p)
        gtiSdkLib.GtiOpenDevice(self.obj, deviceName.encode('ascii'))
        # Initialization.
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiInitialization.argtypes = (c_ulong,)
        else:
            gtiSdkLib.GtiInitialization.argtypes = (c_ulonglong,)
        ret = gtiSdkLib.GtiInitialization(self.obj)
        return ret

    @Deprecated
    def ReopenDevice(self, deviceName):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiCloseDevice.argtypes = (c_ulong,)
        else:
            gtiSdkLib.GtiCloseDevice.argtypes = (c_ulonglong,)
        gtiSdkLib.GtiCloseDevice(self.obj)
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiOpenDevice.argtypes = (c_ulong, c_char_p)
        else:
            gtiSdkLib.GtiOpenDevice.argtypes = (c_ulonglong, c_char_p)
        return gtiSdkLib.GtiOpenDevice(self.obj, deviceName.encode('ascii'))

    def UnlockDevice(self):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiUnlockDevice.argtypes = (c_ulong,)
        else:
            gtiSdkLib.GtiUnlockDevice.argtypes = (c_ulonglong,)
        return gtiSdkLib.GtiUnlockDevice(self.obj)

    def ResetDevice(self):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiResetDevice.argtypes = (c_ulong,)
        else:
            gtiSdkLib.GtiResetDevice.argtypes = (c_ulonglong,)
        return gtiSdkLib.GtiResetDevice(self.obj)

    def GetDeviceType(self):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiGetDeviceType.argtypes = (c_ulong,)
        else:
            gtiSdkLib.GtiGetDeviceType.argtypes = (c_ulonglong,)
        return gtiSdkLib.GtiGetDeviceType(self.obj)

    def GetDeviceName(self):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiGetDeviceName.argtypes = (c_ulong,)
        else:
            gtiSdkLib.GtiGetDeviceName.argtypes = (c_ulonglong,)
        return gtiSdkLib.GtiGetDeviceName(self.obj)

    def GetDevicePlatformName(self):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiGetDevicePlatformName.argtypes = (c_ulong,)
        else:
            gtiSdkLib.GtiGetDevicePlatformName.argtypes = (c_ulonglong,)
        return gtiSdkLib.GtiGetDevicePlatformName(self.obj)

    def CheckDeviceStatus(self):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiCheckDeviceStatus.argtypes = (c_ulong,)
        else:
            gtiSdkLib.GtiCheckDeviceStatus.argtypes = (c_ulonglong,)
        return gtiSdkLib.GtiCheckDeviceStatus(self.obj)

    def LoadModel(self, modelUrl, mode, networkId):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiLoadModel.argtypes = (c_ulong, c_char_p, c_int, c_int)
        else:
            gtiSdkLib.GtiLoadModel.argtypes = (c_ulonglong, c_char_p, c_int, c_int)
        return gtiSdkLib.GtiLoadModel(self.obj, modelUrl.encode('ascii'), mode, networkId)

    def ChangeModelMode(self, mode):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiChangeModelMode.argtypes = (c_ulong, c_int)
        else:
            gtiSdkLib.GtiChangeModelMode.argtypes = (c_ulonglong, c_int)
        return gtiSdkLib.GtiChangeModelMode(self.obj, mode)

    def ChangeModelNetworkId(self, networkId):
        if(ARCH_SIZE == 4):
            gtiSdkLib.GtiChangeModelNetworkId.argtypes = (c_ulong, c_int)
        else:
            gtiSdkLib.GtiChangeModelNetworkId.argtypes = (c_ulonglong, c_int)
        return gtiSdkLib.GtiChangeModelNetworkId(self.obj, networkId)

    @Deprecated
    def DeviceRelease(self):
        return self.GtiDeviceRelease()

