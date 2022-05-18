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

gdev = gtilib.GtiDevice(deviceType, glcoef_file, glconfig_file)

fcCoef = glGnet32PicCoefDataFcFileName
fcLabel = glGnet32PicLabelFileName

# Open GTI device
ret = gdev.OpenDeviceAndInit(deviceName)

#print "Classify construct."
clf = gc.Classify(fcCoef, fcLabel)

outLen = gdev.GetOutputLength()
outBuffArray = np.arange(outLen, dtype=np.float)
outBuff = outBuffArray.tostring()

# create a display window:
cv2.namedWindow("image", cv2.WINDOW_NORMAL)
cv2.resizeWindow("image", 800, 600)
cv2.moveWindow("image", 400, 100)

while True:
    root = Tk()
    root.withdraw()
    filename = askopenfilename(parent = root, title = "Open File", initialdir = glPictureDir)
    if type(filename) == tuple:
        break
    print "file name ", filename
    img = cv2.imread(filename)
    if type(img) == type(None):
        break

    img1 = cv2.resize(img, (GTI_IMAGE_WIDTH, GTI_IMAGE_HEIGHT))
    b,g,r = cv2.split(img1)
    b2 = np.concatenate((b, g, r))
    #print "Handle one frame"
    ret = gdev.HandleOneFrameFloat(b2.tostring(), GTI_IMAGE_WIDTH * GTI_IMAGE_HEIGHT * 3, outBuff, outLen)
    if ret == 0:
        gdev.ReopenDevice(deviceName)

    predication = clf.gClassify(outBuff, num)

    label1 = " "
    label2 = " "
    label1 = clf.gGetPredicationString(0)
    label2 = clf.gGetPredicationString(1)
    print label1
    print label2

    img2 = cv2.resize(img, (800, 600))
    fontface = cv2.FONT_HERSHEY_SIMPLEX
    cv2.putText(img2, label1, (10,17), fontface, 0.7, (0,0,255), 2, 8)
    cv2.putText(img2, label2, (10,38), fontface, 0.7, (0,255,255), 2, 8)
    cv2.imshow('image', img2)

    key = cv2.waitKey(0)
    if (key&0xff) == 113: #ord('q'):
        break

cv2.destroyAllWindows()
cv2.waitKey(10)
#print "Close device."
gdev.CloseDevice()
gdev.GtiDeviceRelease()
clf.gClassifyRelease()

