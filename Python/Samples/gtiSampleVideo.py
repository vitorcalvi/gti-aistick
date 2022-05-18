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

fcCoef = glGnet32VideoCoefDataFcFileName
fcLabel = glGnet32VideoLabelFileName

# Open GTI device
ret = gdev.OpenDeviceAndInit(deviceName)

#print "Classify construct."
clf = gc.Classify(fcCoef, fcLabel)

outLen = gdev.GetOutputLength()
outBuffArray = np.arange(outLen, dtype=np.float)
outBuff = outBuffArray.tostring()

# create a display window:
cv2.namedWindow("Video", cv2.WINDOW_NORMAL)
cv2.resizeWindow("Video", 800, 600)
cv2.moveWindow("Video", 400, 100)

while True:
    root = Tk()
    root.withdraw()
    filename = askopenfilename(parent = root, title = "Open File", initialdir = glVideoDir)
    if type(filename) == tuple:
        break

    cap = cv2.VideoCapture(filename)
    if cap.isOpened() == 0:
        print ("Video Clip File ", filename, " is not opened!")
        break

    posFrame = cap.get(cv2.CAP_PROP_POS_FRAMES)
    maxFrameNum = cap.get(cv2.CAP_PROP_FRAME_COUNT)
    frameNum = 0
    while frameNum < maxFrameNum:
        ret, img = cap.read()
        if ret == 0:
            cap.set(cv2.CAP_PROP_POS_FRAMES, posFrame-1)
            cv2.waitKey(200)
            ret, img = cap.read()

        posFrame = cap.get(cv2.CAP_PROP_POS_FRAMES)
        img1 = cv2.resize(img, (GTI_IMAGE_WIDTH, GTI_IMAGE_HEIGHT))
        b,g,r = cv2.split(img1)
        b2 = np.concatenate((b, g, r))

        ret = gdev.HandleOneFrameFloat(b2.tostring(), 224 * 224 * 3, outBuff, outLen)
        if ret == 0:
            gdev.ReopenDevice(deviceName)
        start = time.time()
        predication = clf.gClassify(outBuff, num)
        print ("time:", time.time() - start)

        label1 = clf.gGetPredicationString(0)
        label2 = clf.gGetPredicationString(1)
        #print label1
        #print label2

        #img2 = cv2.resize(img, (800, 600))
        fontface = cv2.FONT_HERSHEY_SIMPLEX
        cv2.putText(img, label1, (10,20), fontface, 0.8, (0,0,255), 2, 8)
        cv2.putText(img, label2, (10,45), fontface, 0.8, (0,255,255), 2, 8)
        cv2.imshow('Video', img)

        key = cv2.waitKey(1) & 0xff
        if key == 32:
            cv2.waitKey(0)
        if key == ord('q') or key == 27:
            break

        frameNum += 1
        if frameNum >= maxFrameNum:
            cap.set(cv2.CAP_PROP_POS_FRAMES, 0);
            frameNum = 0
    cap.release()

cv2.destroyAllWindows()
cv2.waitKey(10)
#print "Close device."
gdev.CloseDevice()
gdev.GtiDeviceRelease()
clf.gClassifyRelease()

