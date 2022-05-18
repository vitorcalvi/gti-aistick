#***********************************************************************
#
# Copyright (c) 2017-2019 Gyrfalcon Technology Inc. All rights reserved.
# See LICENSE file in the project root for full license information.
#
#************************************************************************

from glconfig import *

On = 1
Off = 0
ftdi = 0
eusb = 1
pcie = 2

#USE_FTDI
#deviceType = ftdi
#deviceName = '0'

#USE_EUSB
deviceType = eusb
deviceName = str('/dev/sg2').encode('ASCII')

#USE_PCIE
#deviceType = pcie
#deviceName = '0'

glcoef_file = gnet32coef512
glconfig_file = gnet32config

GTI_IMAGE_WIDTH = 224
GTI_IMAGE_HEIGHT = 224

# Predication output number
num = 5

# CNN mode: 0 - FC mode, 1 - Learning mde, 2 - Single layer 5-3 output mode, 3 - sub last mode (1-2,2-2,3-3,4-3,5-3)
cnnMode = 0

# Set Caffe mode: 0: Gnet1 (VGG16), 1: Gnet2, 2: Gnet3
gnetType = 2
gdev = gtilib.GtiDevice(deviceType, glcoef_file, glconfig_file)

if gnetType == 1 or gnetType == 2:
    if cnnMode == 2:
        coefName = gnet32coefFile
    else:
        coefName = gnet32coef512
    fcCoef = glGnet32WebCamCoefDataFcFileName
    fcLabel = glGnet32WebCamLabelFileName
else:
    coefName = coefFilename
    fcCoef = glWebCamCoefDataFcFileName
    fcLabel = glWebCamLabelFileName

# Open GTI device
ret = gdev.OpenDeviceAndInit(deviceName)

#print "Classify construct."
clf = gc.Classify(fcCoef, fcLabel)

outLen = gdev.GetOutputLength()
outBuffArray = np.arange(outLen, dtype=np.float)
outBuff = outBuffArray.tostring()

# create a display window:
cv2.namedWindow("WebCam", cv2.WINDOW_NORMAL)
cv2.resizeWindow("WebCam", 800, 600)
cv2.moveWindow("WebCam", 400, 100)

index = input("Input WebCam port number (0, 1, 2, ...): ")
cap = cv2.VideoCapture(int(index))
if cap.isOpened() == 0:
    print "Web Cam Device ",index," is not opened!"

while True:
    ret, img = cap.read()
    if ret == False:
        break

    img1 = cv2.resize(img, (GTI_IMAGE_WIDTH, GTI_IMAGE_HEIGHT))
    b,g,r = cv2.split(img1)
    b2 = np.concatenate((b, g, r))

    ret = gdev.HandleOneFrameFloat(b2.tostring(), 224 * 224 * 3, outBuff, outLen)
    if ret == 0:
        gdev.ReopenDevice(deviceName)

    predication = clf.gClassify(outBuff, num)

    label1 = clf.gGetPredicationString(0)
    label2 = clf.gGetPredicationString(1)
    #print label1
    #print label2

    img2 = cv2.resize(img, (800, 600))
    fontface = cv2.FONT_HERSHEY_SIMPLEX
    cv2.putText(img2, label1, (10,17), fontface, 0.7, (0,0,255), 2, 8)
    cv2.putText(img2, label2, (10,38), fontface, 0.7, (0,255,255), 2, 8)
    cv2.imshow('WebCam', img2)

    if cv2.waitKey(1) & 0xff == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
cv2.waitKey(10)
#print "Close device."
gdev.CloseDevice()
gdev.GtiDeviceRelease()
clf.gClassifyRelease()

