from gtilib import *
import gtiClassify as gc
import numpy as np
import os
import ctypes

"""
This script is modified based on ../../Python/Samples/gtiLitePic.py
to demonstrate equivalency between SDK 3.1 and SDK 4.x API
gtiClassify module is not needed in SDK 4.x (please see liteDemo.py)
but keep it here for showing the equivalency.

Before running, please prepare your SDK 4.x model in the following way:

mkdir ../../Apps/Models/2801/gnet2_3/  # crate a new dir for the new model
cd ../../Apps/Models/2801/gnet2_3/
cp ../../../../Tools/modelTool/modelTool . #get the model conversion tool
cp /usr/local/GTISDKPATH/data/Models/gti2801/gnet2_3/cnn_3/gnet32_coef_512.dat gti_gnet32_coef_512.model.1
cat << EOF > gti_gnet32_coef_512.model.0
{
   "name": "gti_gnet32_coef_512",
   "version": "4.0.0",
   "data hash": null,
   "layer": [
        {
            "data file": "gti_gnet32_coef_512.model.1",
            "data offset": 0,
            "device": {
                "chip": "2801",
                "emmc delay": 15000,
                "name": null,
                "type": 0
            },
            "name": "cnn",
            "operation": "GTICNN",
            "output channels": 256,
            "output height": 7,
            "output width": 7
        }
    ]
}
EOF
./modelTool modelenc gti_gnet32_coef_512.model.0 # create a new model with name gti_gnet32_coef_512.model.0.gti
cp /usr/local/GTISDKPATH/data/Models/gti2801/gnet2_3/fc/gnet32_fc128_20class.bin .
cp /usr/local/GTISDKPATH/data/Models/gti2801/gnet2_3/fc/gnet32_fc128_20class_label.txt . # copy fc data files
"""

# Input picture is the same as /usr/local/GTISDKPATH/data/Image_Lite/bridge.bin
# picture directory
glPictureDir = str("../../Apps/Data" + "/Image_lite/").encode('ascii')
glImagename = str(glPictureDir + "bridge_c20.bin").encode('ascii')

# CNN model is equivalent to the original /usr/local/GTISDKPATH/data/Models/gti2801/gnet2_3/cnn_3/
# Gnet root
gnet2_3root = str("../../Apps/" + "/Models/2801/gnet2_3/").encode('ascii')
#gnet2_3root = str("../../Apps/" + "/Models/2801/gnet1/").encode('ascii')
# Common coef filter file
gnet32ModelFile = str(gnet2_3root + "gti_gnet32_coef_512.model.0.gti").encode('ascii')
#gnet32ModelFile = str(gnet2_3root + "gti_gnet1_coef_1000.model.0.gti").encode('ascii')
#glconfig_file = str(gnet2_3root + "userinput.txt").encode('ascii')
#gnet32coef512 = str(gnet2_3root + "gnet32_coef_512.dat").encode('ascii')
# Files for picture
glGnet32PicCoefDataFcFileName = str(gnet2_3root + "gnet32_fc128_20class.bin").encode('ascii')
glGnet32PicLabelFileName = str(gnet2_3root + "gnet32_fc128_20class_label.txt").encode('ascii')
#glGnet32PicCoefDataFcFileName = str(gnet2_3root + "picture_coef.bin").encode('ascii')
#glGnet32PicLabelFileName = str(gnet2_3root + "picture_label.txt").encode('ascii')

GTI_IMAGE_WIDTH = 224
GTI_IMAGE_HEIGHT = 224

print( "Open GTI Model file : %s" % gnet32ModelFile)
model=GtiModel(gnet32ModelFile)

fcCoef = glGnet32PicCoefDataFcFileName
fcLabel = glGnet32PicLabelFileName
clf = gc.Classify(fcCoef, fcLabel)

print "file name ", glImagename
with open(glImagename, 'rb') as fp:
    buf = fp.read(GTI_IMAGE_WIDTH*GTI_IMAGE_HEIGHT*3)
    out_b, out_l,w_out,h_out,d_out = model.GtiEvaluateFloatOut( buf, GTI_IMAGE_WIDTH, GTI_IMAGE_HEIGHT, 3 )
    predication = clf.gClassifyFloatIn(out_b, 5)
    label1 = clf.gGetPredicationString(0)
    label2 = clf.gGetPredicationString(1)
    print label1
    print label2

clf.gClassifyRelease()

