#/***********************************************************************
#*
#* Copyright (c) 2017-2019 Gyrfalcon Technology Inc. All rights reserved.
#* See LICENSE file in the project root for full license information.
#*
#************************************************************************/

import sys
import os
import glob 
import platform

import cv2
import numpy as np
import sys
import json

from gtilib import *

GTI_IMAGE_WIDTH = 224
GTI_IMAGE_HEIGHT = 224

SHOW_IMAGE_WIDTH = 800
SHOW_IMAGE_HEIGHT = 600

WINDOW_TITLE='Demo'

def printUsage():
    s="Usage: python "+sys.argv[0]+" command model_file [image/video/dir/...]\n";
    s+="  Ex: python "+sys.argv[0]+" image     ../Models/.../gti_gnet1.model ../Data/Image_bmp_c1000/truck.bmp\n";
    s+="      python "+sys.argv[0]+" video     ../Models/.../gti_mnet.model ../Data/Image_mp4/video_40class.avi\n";
    s+="      python "+sys.argv[0]+" camera    ../Models/.../gti_gnet3.model 0\n";
    s+="      python "+sys.argv[0]+" slideshow ../Models/.../gti_gnet3.model ../Data/Image_bmp_c20/\n";
    print(s)
    sys.exit(-1)


def loadModel(file):
    try:
        md=GtiModel(file)
    except Exception as e:
        print str(e)
        sys.exit(-1)
    return md

def processImage(img, model, title):
    img1 = cv2.resize(img, (GTI_IMAGE_WIDTH, GTI_IMAGE_HEIGHT))
    b,g,r = cv2.split(img1)
    b2 = np.concatenate((b, g, r))
    jd = model.GtiImageEvaluate(b2.tostring(), GTI_IMAGE_WIDTH, GTI_IMAGE_HEIGHT, 3)
    jr=json.loads(jd)
    result_list = jr["result"]

    label1 = result_list[0]["label"]

    img2 = cv2.resize(img, (800, 600))
    fontface = cv2.FONT_HERSHEY_SIMPLEX
    cv2.putText(img2, label1, (10,15), fontface, 0.6, 255, 1, 8)
    cv2.imshow(title, img2)
   
def showImage(argv):
    md=loadModel(argv[1])
    img = cv2.imread(argv[2])
    if type(img) == type(None):
        print('could not read image file:',argv[2])
        sys.exit(-1) 
    processImage(img,md,WINDOW_TITLE)
    for i in range(0,1000) :
       key = cv2.waitKey(10)
       if (key=='q') or (key==113) :
                break

def resizeCaptureWindow(cap,title):
    w=min(SHOW_IMAGE_WIDTH,cap.get(cv2.CAP_PROP_FRAME_WIDTH))
    h=min(SHOW_IMAGE_HEIGHT,cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
    cv2.resizeWindow(title, int(w), int(h))
    
def playVideo(argv):
    md=loadModel(argv[1])
    cap =cv2.VideoCapture(argv[2])
    if cap.isOpened() == False :
        print("could not open video file:", argv[2])
        sys.exit(-1)
    resizeCaptureWindow(cap,WINDOW_TITLE)
    while True :
        flag,img=cap.read()
        if flag==False :
            cap.set(cv2.CAP_PROP_POS_FRAMES,0)
            continue
        processImage(img,md,WINDOW_TITLE)
        key=cv2.waitKey(1) & 0xff
        if key==' ' or key==32:
            key=cv2.waitKey(0) 
        if (key=='q') or (key==113) :
            break
    cap.release()
 
def showCamera(argv):
    md=loadModel(argv[1])
    cap =cv2.VideoCapture(int(argv[2]))
    if cap.isOpened() == False :
        print("could not open camera:", argv[2])
        sys.exit(-1)
    resizeCaptureWindow(cap,WINDOW_TITLE)
    while True :
            flag,img=cap.read()
            processImage(img,md,WINDOW_TITLE)
            key=cv2.waitKey(1) & 0xff
            if (key=='q') or (key==113) :
                break
    cap.release()
   
def slideShow(argv):
    md=loadModel(argv[1])
    for filename in glob.glob(argv[2]+'/*'): 
        print filename
        img = cv2.imread(filename)
        if type(img) == type(None):
            print('could not read image file:',filename)
            continue 
        processImage(img,md,WINDOW_TITLE)
        key = cv2.waitKey(1000)
        if (key==' ') or (key==32) :
            key=cv2.waitKey(0) 
        if (key=='q') or (key==113) :
            break

def main(argv):
    if argv[0]=='image':
        showImage(argv)
    elif argv[0]=='video':
        playVideo(argv)
    elif argv[0]=='camera':
        showCamera(argv)
    elif argv[0]=='slideshow':
        slideShow(argv)
    else:
        printUsage()
        

if __name__=="__main__":
    if(len(sys.argv)<4):
        printUsage()
    cv2.namedWindow(WINDOW_TITLE, cv2.WINDOW_AUTOSIZE)
    cv2.resizeWindow(WINDOW_TITLE, SHOW_IMAGE_WIDTH, SHOW_IMAGE_HEIGHT)
    cv2.moveWindow(WINDOW_TITLE, 400, 100)
    main(sys.argv[1:])
    cv2.destroyAllWindows()

