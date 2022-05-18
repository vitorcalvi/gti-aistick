#***********************************************************************
#
# Copyright (c) 2017-2019 Gyrfalcon Technology Inc. All rights reserved.
# See LICENSE file in the project root for full license information.
#
#************************************************************************

from gtilib import *
from gtiClassify import *

import gtilib
import gtiClassify as gc
import os
import sys

CV2LIB_PATH=str("../../Lib/" + OS_TYPE + "/OpenCV/" + CPU_ARCH + "/").encode('ascii') 
sys.path.append(CV2LIB_PATH)

import cv2
import numpy as np
import time

from Tkinter import *
from tkFileDialog import *
from struct import *

glGtiPath=os.getenv('GTISDKPATH')
if glGtiPath == None:
    raise ValueError('GTISDKPATH path is not set!')

datapath="/data/"

# picture directory
glPictureDir = str(glGtiPath + datapath + "Image_bmp/").encode('ascii')
# Video directory
glVideoDir = str(glGtiPath + datapath + "Image_mp4/").encode('ascii')


# Gnet root
gnet1root = str(glGtiPath + datapath + "Models/gti2801/gnet1/").encode('ascii')
gnet2_3root = str(glGtiPath + datapath + "Models/gti2801/gnet2_3/").encode('ascii')

# Common coef filter file
coefFilename = str(gnet1root + "cnn/gnet1_coef_vgg16.dat").encode('ascii')
coefconfig = str(gnet1root + "cnn/userinput.txt").encode('ascii')
gnet32coef512 = str(gnet2_3root + "cnn_3/gnet32_coef_512.dat").encode('ascii')
gnet32config = str(gnet2_3root + "cnn_3/userinput.txt").encode('ascii')


# Files for picture
glPicCoefDataFcFileName = str(gnet1root + "fc/picture_coef.bin").encode('ascii')
glPicLabelFileName = str(gnet1root + "fc/picture_label.txt").encode('ascii')

glGnet32PicCoefDataFcFileName = str(gnet2_3root + "fc/gnet32_pic_coef_1000.bin").encode('ascii')
glGnet32PicLabelFileName = str(gnet2_3root + "fc/gnet32_pic_label_1000.txt").encode('ascii')

# Files for video
glVideoCoefDataFcFileName = str(gnet1root + "fc/video_coef.bin").encode('ascii')
glVideoLabelFileName = str(gnet1root + "fc/video_label.txt").encode('ascii')

glGnet32VideoCoefDataFcFileName = str(gnet2_3root + "fc/gnet32_video_coef_40.bin").encode('ascii')
glGnet32VideoLabelFileName = str(gnet2_3root + "fc/gnet32_video_label_40.txt").encode('ascii')

# Files for web camera
glWebCamCoefDataFcFileName = str(gnet1root + "fc/webcam_coef.bin").encode('ascii')
glWebCamLabelFileName = str(gnet1root + "fc/webcam_label.txt").encode('ascii')

glGnet32WebCamCoefDataFcFileName = str(gnet2_3root + "fc/gnet32_20class.bin").encode('ascii')
glGnet32WebCamLabelFileName = str(gnet2_3root + "fc/gnet32_20class_label.txt").encode('ascii')
