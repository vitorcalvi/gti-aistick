#***********************************************************************
#
# Copyright (c) 2017-2019 Gyrfalcon Technology Inc. All rights reserved.
# See LICENSE file in the project root for full license information.
#
#************************************************************************


from glconfig import *
import threading
import Queue

On = 1
Off = 0
ftdi = 0
eusb = 1
pcie = 2

# deviceType = ftdi
# deviceName1 = '0'
# deviceName2 = '1'

deviceType1 = ftdi
deviceName1 = str('0').encode('ASCII')

deviceType2 = eusb
deviceName2 = str('/dev/sg2').encode('ASCII')

glcoef_file = gnet32coef512
glconfig_file = gnet32config

GTI_IMAGE_WIDTH = 224
GTI_IMAGE_HEIGHT = 224

# create a display window:
cv2.namedWindow("Video1", cv2.WINDOW_NORMAL)
cv2.resizeWindow("Video", 700, 460)
cv2.moveWindow("Video1", 0, 0)
cv2.namedWindow("Video2", cv2.WINDOW_NORMAL)
cv2.resizeWindow("Video2", 700, 460)
cv2.moveWindow("Video2", 0, 520)


class myThread (threading.Thread):
    def __init__(self, threadID, threadName, gdev, gclf, gOutBuf, gOutLen):
        threading.Thread.__init__(self)
        self.threadID = threadID
        self.name = threadName
        self.gdev = gdev
        self.gclf = gclf
        self.gOutBuf = gOutBuf
        self.gOutLen = gOutLen

    def run(self):
        while loopFlag == 1:
            if self.threadID == 1:
                if not workQueue1.empty():
                    gInbuf = workQueue1.get()
                    cnnAndFc(self.gdev, self.gclf, gInbuf, self.gOutBuf, self.gOutLen)
                    workQueueOut1.put(self.gOutLen)
            if self.threadID == 2:
                if not workQueue2.empty():
                    gInbuf = workQueue2.get()
                    cnnAndFc(self.gdev, self.gclf, gInbuf, self.gOutBuf, self.gOutLen)
                    workQueueOut2.put(self.gOutLen)

def cnnAndFc(gdev, gclf, gInBuf, gOutBuf, gOutLen):
    num = 5
    ret = gdev.HandleOneFrameFloat(gInBuf, 224 * 224 * 3, gOutBuf, gOutLen)
#    if ret == 0:
#        glib.ReopenDevice(deviceName)
    predication = gclf.gClassify(gOutBuf, num)


# Main()

loopFlag = 1

# CNN mode: 0 - FC mode, 1 - Learning mde, 2 - Single layer 5-3 output mode, 3 - sub last mode (1-2,2-2,3-3,4-3,5-3)
cnnMode = 0

# Set Caffe mode: 0: Gnet1 (VGG16), 1: Gnet2, 2: Gnet3
gnetType = 2

# Predication output number
pool5 = 0

# Create GTI device for Chip1
gdev1 = gtilib.GtiDevice(deviceType1, glcoef_file, glconfig_file)

# Create GTI device for Chip2
gdev2 = gtilib.GtiDevice(deviceType2, glcoef_file, glconfig_file)

if gnetType == 1 or gnetType == 2:
    if cnnMode == 2:
        coefName = gnet32coefFile
    else:
        coefName = gnet32coef512
    fcCoef = glGnet32VideoCoefDataFcFileName
    fcLabel = glGnet32VideoLabelFileName
else:
    coefName = coefFilename
    fcCoef = glVideoCoefDataFcFileName
    fcLabel = glVideoLabelFileName

# Open GTI device1
ret = gdev1.OpenDeviceAndInit(deviceName1)
clf1 = gc.Classify(fcCoef, fcLabel)
# Open GTI device2
ret = gdev2.OpenDeviceAndInit(deviceName2)
clf2 = gc.Classify(fcCoef, fcLabel)

# Output buffer for chip1
outLen1 = gdev1.GetOutputLength()
outBuffArray1 = np.arange(outLen1, dtype=np.float)
outBuff1 = outBuffArray1.tostring()
# Output buffer for chip2
outLen2 = gdev2.GetOutputLength()
outBuffArray2 = np.arange(outLen2, dtype=np.float)
outBuff2 = outBuffArray2.tostring()

queueSize = 2
workQueue1 = Queue.Queue(queueSize)
workQueue2 = Queue.Queue(queueSize)
workQueueOut1 = Queue.Queue(queueSize)
workQueueOut2 = Queue.Queue(queueSize)

threads = []
threadIDChip1 = 1
threadIDChip2 = 2

threadChip1 = myThread(threadIDChip1, "Video Thread Chip1", gdev1, clf1, outBuff1, outLen1)
threads.append(threadChip1)
threadChip2 = myThread(threadIDChip2, "Video Thread Chip2", gdev2, clf2, outBuff2, outLen2)
threads.append(threadChip2)

threadChip1.start()
threadChip2.start()

fpsCount = 10

key = ord(' ')
os.chdir(glVideoDir)
cwd = os.getcwd()
while True:
    root = Tk()
    root.withdraw()
    filename1 = askopenfilename(parent = root, title = "Open File", initialdir = glVideoDir)
    if type(filename1) == tuple:
        break
    print ("file1 name ",filename1)

    filename2 = askopenfilename(parent = root, title = "Open File", initialdir = glVideoDir)
    if type(filename2) == tuple:
        break
    print ("file2 name ",filename2)

    cap1 = cv2.VideoCapture(filename1)
    cap2 = cv2.VideoCapture(filename2)
    
    if cap1.isOpened() == 0:
        print "Video Clip1 File ", filename1, " is not opened!"
        break

    if cap2.isOpened() == 0:
        print "Video Clip1 File ", filename2, " is not opened!"
        break

    width1 = cap1.get(cv2.CAP_PROP_FRAME_WIDTH)   # float
    height1 = cap1.get(cv2.CAP_PROP_FRAME_HEIGHT) # float
    #cv2.resizeWindow("Video1", int(width1), int(height1))
    cv2.resizeWindow("Video1", 700, 460)

    width2 = cap2.get(cv2.CAP_PROP_FRAME_WIDTH)   # float
    height2 = cap2.get(cv2.CAP_PROP_FRAME_HEIGHT) # float
    #cv2.resizeWindow("Video2", int(width2), int(height2))
    cv2.resizeWindow("Video2", 700, 460)

    posFrame1 = cap1.get(cv2.CAP_PROP_POS_FRAMES)
    maxFrameNum1 = cap1.get(cv2.CAP_PROP_FRAME_COUNT)
    frameNum1 = 0
    fps1 = 0
    label1 = " "
    label2 = " "
    timerStart1 = time.time()
    timerBegin1 = timerStart1
    timerCount1 = timerStart1

    posFrame2 = cap2.get(cv2.CAP_PROP_POS_FRAMES)
    maxFrameNum2 = cap1.get(cv2.CAP_PROP_FRAME_COUNT)
    frameNum2 = 0
    fps2 = 0
    label3 = " "
    label4 = " "
    timerStart2 = time.time()
    timerBegin2 = timerStart2
    timerCount2 = timerStart2

    while True:
        ret, img_1 = cap1.read()
        if ret == 0:
            cap1.set(cv2.CAP_PROP_POS_FRAMES, posFrame1-1)
            cv2.waitKey(200)
            ret, img_1 = cap1.read()
                
        img1 = cv2.resize(img_1, (GTI_IMAGE_WIDTH, GTI_IMAGE_HEIGHT))
        b,g,r = cv2.split(img1)
        b1 = np.concatenate((b, g, r))
        workQueue1.put(b1.tostring())

        ret, img_2 = cap2.read()
        if ret == 0:
            cap2.set(cv2.CAP_PROP_POS_FRAMES, posFrame2-1)
            cv2.waitKey(200)
            ret, img_2 = cap2.read()
            
        img2 = cv2.resize(img_2, (GTI_IMAGE_WIDTH, GTI_IMAGE_HEIGHT))
        b,g,r = cv2.split(img2)
        b2 = np.concatenate((b, g, r))
        workQueue2.put(b2.tostring())
            
        while True:
            if not workQueueOut1.empty():
                workQueueOut1.get()
                break
        label1 = clf1.gGetPredicationString(0)
        label2 = clf1.gGetPredicationString(1)

        while True:
            if not workQueueOut2.empty():
                workQueueOut2.get()
                break
        label3 = clf2.gGetPredicationString(0)
        label4 = clf2.gGetPredicationString(1)
                
        fontface = cv2.FONT_HERSHEY_SIMPLEX
        cv2.putText(img_1, "FPS: " + str(fps1), (10,20), fontface, 0.8, 255, 2, 8)
        cv2.putText(img_1, label1, (10,45), fontface, 0.8, (0,0,255), 2, 8)
        cv2.putText(img_1, label2, (10,70), fontface, 0.8, (0,255,255), 2, 8)
        cv2.imshow('Video1', img_1)

        cv2.putText(img_2, "FPS: " + str(fps2), (10,20), fontface, 0.8, 255, 2, 8)
        cv2.putText(img_2, label1, (10,45), fontface, 0.8, (0,0,255), 2, 8)
        cv2.putText(img_2, label2, (10,70), fontface, 0.8, (0,255,255), 2, 8)
        cv2.imshow('Video2', img_2)

        key = cv2.waitKey(1) & 0xff
　　　　　　　　if key == 32:
            cv2.waitKey(0)
        if key == ord('q') or key == 27:
            break

        frameNum1 += 1
        if frameNum1 % fpsCount == 0:
            timerStop = time.time()
            fps1 = fpsCount / (timerStop - timerStart1)
            timerStart1 = time.time()
            
        if frameNum1 >= maxFrameNum1:
            cap1.set(cv2.CAP_PROP_POS_FRAMES, 0);
            frameNum1 = 0

        frameNum2 += 1
        if frameNum2 % fpsCount == 0:
            timerStop = time.time()
            fps2 = fpsCount / (timerStop - timerStart2)
            timerStart2 = time.time()
            
        if frameNum2 >= maxFrameNum2:
            cap2.set(cv2.CAP_PROP_POS_FRAMES, 0);
            frameNum2 = 0

    cap1.release()
    cap2.release()

loopFlag = 0
# Wait for all threads to complete
for t in threads:
   t.join()

cv2.destroyAllWindows()
cv2.waitKey(10)
#print "Close device."
gdev1.CloseDevice()
gdev1.GtiDeviceRelease()
clf1.gClassifyRelease()
gdev2.CloseDevice()
gdev2.GtiDeviceRelease()
clf2.gClassifyRelease()

