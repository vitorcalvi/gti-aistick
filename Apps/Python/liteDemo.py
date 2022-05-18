import cv2
import numpy as np
import json

from gtilib import *

GTI_IMAGE_WIDTH = 224
GTI_IMAGE_HEIGHT = 224

md = GtiModel("../Models/2801/gti_gnet3_fc20_2801.model")

img = cv2.imread("../Data/Image_bmp_c20/bridges.jpg")
img1 = cv2.resize(img, (GTI_IMAGE_WIDTH, GTI_IMAGE_HEIGHT))
b,g,r = cv2.split(img1)
b2 = np.concatenate((b, g, r))

jd = md.GtiImageEvaluate( b2.tostring(), GTI_IMAGE_WIDTH, GTI_IMAGE_HEIGHT, 3 )
jr=json.loads(jd)
result_list = jr["result"]
label1 = result_list[0]["label"]

print( "Expecting \"bridge\"" )
print( label1 )


