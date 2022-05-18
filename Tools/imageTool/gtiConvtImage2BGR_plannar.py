import cv2
import sys

class convtImage(object):
    def __init__(self, input_image, image_size, output_bin):
        if type(input_image) == tuple:
            return
        imageSize = int(image_size)
        print( "Image file : " + input_image)
        print( "Image width: " + str(imageSize))
        print( "Output file: " + output_bin)
        img = cv2.imread(input_image)
        if img is None:
            print ("Error: input image " + input_image + " not exist!")
            return

        image = cv2.resize(img, (imageSize, imageSize))	
        b,g,r = cv2.split(image)

        fdata = open(output_bin, "wb")

        fdata.write(b)
        fdata.write(g)
        fdata.write(r)

        fdata.close()
        print ("BGR plannar format image file " + output_bin + " is created.")

if __name__ == '__main__': # (sys.argv):
    if len(sys.argv) < 4:
        print ("python " + sys.argv[0] + " <inputImage> <image_width> <output_bin>")
        exit(0)
	
    convtImage(sys.argv[1], sys.argv[2], sys.argv[3])

