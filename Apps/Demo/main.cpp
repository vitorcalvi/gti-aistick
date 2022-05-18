
/***********************************************************************
*
* Copyright (c) 2017-2019 Gyrfalcon Technology Inc. All rights reserved.
* See LICENSE file in the project root for full license information.
*
************************************************************************/

#include <iostream>
#include <chrono>
#include <cmath>
#include "json.hpp"
#include <GTILib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>

using namespace cv;
using namespace std;
using json = nlohmann::json;

typedef void (*commandFn)(GtiModel *, const char *, const int);

#define IMAGE_SIZE 448 // this is the maximum image size
int image_size = 224;  // this is the actual image size
#define IMAGE_DEPTH 3
#define WINDOW_SIZE 800

char *gBuffer[IMAGE_SIZE * IMAGE_SIZE * IMAGE_DEPTH];
static bool showFPS = 0;
static float inferenceTime = 0, fps = 0;


void printUsage(GtiModel *model, const char *appName, const int dummy) {
    cout << "Usage: " << appName << " command model_file [image|videod|dir|0] [224|448]]" << endl;
    cout << "   Ex: " << appName << " image     ../Models/.../gti_gnet1.model ../Data/Image_bmp_c1000/truck.bmp"
         << endl;
    cout << "       " << appName << " video     ../Models/.../gti_mnet.model ../Data/Image_mp4/video_40class.avi"
         << endl;
    cout << "       " << appName << " camera    ../Models/.../gti_gnet3.model 0" << endl;
    cout << "       " << appName << " slideshow ../Models/.../gti_gnet3.model ../Data/Image_bmp_c20/" << endl;
    exit(-1);
}

void setLabel(Mat &im, string &label, const Point &orig, const double scale) {
    int fontface = cv::FONT_HERSHEY_COMPLEX_SMALL;
    int thickness = scale;
    int baseline = 0;
    json labelJson = json::parse(label);
    string fullLabel = labelJson["result"][0]["label"];
    cout << labelJson["result"][0] << endl;
    string displayText = fullLabel.substr(fullLabel.find_first_of(" ") + 1);

    Size text = cv::getTextSize(displayText, fontface, scale, thickness, &baseline);
    rectangle(im, orig + cv::Point(0, baseline), orig + cv::Point(text.width, -text.height), CV_RGB(50, 60, 70),
              CV_FILLED);
    putText(im, displayText, orig, fontface, scale, CV_RGB(255, 255, 0), thickness, 8);

    char secondLine[50];
    sprintf(secondLine, "Inference time=%3.2fms, FPS=%3.1f", inferenceTime, fps);
    displayText = string(secondLine);
    text = cv::getTextSize(displayText, fontface, scale, thickness, &baseline);
    rectangle(im, orig + cv::Point(0, baseline + text.height * 3 / 2), orig + cv::Point(text.width, text.height / 2),
              CV_RGB(50, 60, 70), CV_FILLED);
    putText(im, displayText, orig + cv::Point(0, text.height * 3 / 2), fontface, scale, CV_RGB(0, 255, 0), thickness,
            8);
    //    cout << displayText.c_str() << endl;
}

void processImage(Mat &img, GtiModel *model, const char *title, const int imgsize) {
    static float inferenceTime_us;
    static int cnt = 0;
    Mat img1;
    GtiTensor input = {0};
    input.height = input.width = imgsize;
    input.depth = IMAGE_DEPTH;
    // unique_ptr<char[]> buffer(new char[input.height*input.height*input.width]);
    input.buffer = gBuffer; // buffer.get();
    resize(img, img1, Size(input.height, input.width));
    std::vector<Mat> splitBGR;
    split(img1, splitBGR);
    memcpy(input.buffer, splitBGR[0].data, imgsize * imgsize);
    memcpy((char *)input.buffer + imgsize * imgsize, splitBGR[1].data, imgsize * imgsize);
    memcpy((char *)input.buffer + imgsize * imgsize * 2, splitBGR[2].data, imgsize * imgsize);

    auto t0=chrono::high_resolution_clock::now();
    GtiTensor *output = GtiEvaluate(model, &input);
    auto t1=chrono::high_resolution_clock::now();
    auto duration=chrono::duration_cast<chrono::duration<float>>(t1-t0);
    inferenceTime_us += 1000000*duration.count()+ numeric_limits<float>::epsilon();
    cnt++;
    if (showFPS) {
        inferenceTime = (inferenceTime_us / 1000 / cnt);
        fps = (1000000 * cnt) / inferenceTime_us;
        cout << endl << "Image inference time = " << inferenceTime << "ms,  FPS = " << fps << endl << endl;
        inferenceTime_us = 0;
        cnt = 0;
        showFPS = 0;
    }
    string label = string((char *)output->buffer);
    // cout << label << endl;
    float scale = 1;
    resize(img, img, Size(WINDOW_SIZE, WINDOW_SIZE * img.rows / img.cols));
    setLabel(img, label, Point(20, 20), scale);
    imshow(title, img);
}

void showImage(GtiModel *model, const char *filename, const int imgsize) {
    Mat img;
    img = imread(filename);
    if (img.empty()) {
        cout << "count not open image file " << filename << endl;
        return;
    }
    auto title = "Image";
    namedWindow(title, WINDOW_NORMAL);
    resizeWindow(title, WINDOW_SIZE, WINDOW_SIZE);
    showFPS = 1;
    processImage(img, model, title, imgsize);
    cout << endl;
}

void slideShow(GtiModel *model, const char *filename, const int imgsize) {
    vector<String> fileArray;
    String folder = filename;
    bool pause = false;
    glob(folder, fileArray);
    if (fileArray.size() == 0) {
        cout << "could not find " << filename << " as a media file/dir" << endl;
    } else {
        while (1) {
            for (auto &i : fileArray) {
                showImage(model, i.c_str(), imgsize);
                int key = waitKey(1) & 0xff;
                if ((key == 'q') || (key == 27))
                    return;
                else if (key == ' ')
                    pause = !pause;
                while (pause == true) {
                    key = waitKey(1) & 0xff;
                    if ((key == 'q') || (key == 27))
                        return;
                    else if (key == ' ')
                        pause = !pause;
                }
            }
        }
    }
}

void playVideo(GtiModel *model, const char *filename, const int imgsize) {
    VideoCapture cap;
    Mat img;
    if (cap.open(filename) && (!cap.isOpened())) {
        cout << "count not open video file " << filename << endl;
        return;
    }
    int w = min(WINDOW_SIZE, (int)cap.get(CV_CAP_PROP_FRAME_WIDTH));
    int h = min(WINDOW_SIZE, (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));
    auto title = "Video";
    namedWindow(title, WINDOW_NORMAL);
    resizeWindow(title, w, h);
    bool state = true;
    int cnt = 0;
    while (1) {
        if (state) {
            if (cap.read(img) == false) {
                cap.set(CV_CAP_PROP_POS_FRAMES, 0);
                continue;
            }
            if (cnt++ > 60) {
                showFPS = 1;
                cnt = 0;
            }
            processImage(img, model, title, imgsize);
        }
        int key = waitKey(1) & 0xff;
        if (key == ' ')
            state = !state;
        else if ((key == 'q') || (key == 27))
            break;
    }
    cap.release();
}

void cameraShow(GtiModel *model, const char *cameraIndex, const int imgsize) {
    VideoCapture cap;
    cap.open(atoi(cameraIndex));
    if (!cap.isOpened()) {
        cout << "count not open camera " << cameraIndex << endl;
        return;
    }
    int w = min(WINDOW_SIZE, (int)cap.get(CV_CAP_PROP_FRAME_WIDTH));
    int h = min(WINDOW_SIZE, (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT));
    const char *title = "camera show";
    namedWindow(title, WINDOW_NORMAL);
    resizeWindow(title, w, h);

    Mat img;
    while (1) {
        cap.grab();
        cap.retrieve(img);
        processImage(img, model, title, imgsize);
        int key = waitKey(1) & 0xff;
        if ((key == 'q') || (key == 27))
            return;
    }
}

int main(int argc, const char *argv[]) {
    map<string, commandFn> commandMap;
    commandMap["image"] = showImage;
    commandMap["video"] = playVideo;
    commandMap["camera"] = cameraShow;
    commandMap["slideshow"] = slideShow;
    commandMap["help"] = printUsage;
    if (argc < 3)
        printUsage(0, argv[0], 0);
    auto it = commandMap.find(argv[1]);
    if (it == commandMap.end()) {
        cout << "supported commands are: ";
        for (auto &i : commandMap)
            cout << i.first << " ";
        cout << endl;
    } else {
        if (argc < 5) {
            image_size = 224;
            cout << "assuming image size is " << image_size << " x " << image_size << endl;
        } else if (argc == 5) {
            int size = stoi(argv[4]);
            if (size == 224 or size == 448) {
                image_size = size;
                cout << "image size is " << image_size << " x " << image_size << endl;
            } else {
                image_size = 224;
                cout << "assuming image size is " << image_size << " x " << image_size << endl;
            }
        }
        if (argc < 4) {
            cout << "please provide media file/dir name" << endl;
            exit(-1);
        }
        const char *modelName = argv[2];
        GtiModel *model = GtiCreateModel(modelName);
        if (0 == model) {
            cout << "count not load Model from " << modelName << endl;
            exit(-1);
        }

        it->second(model, argv[3], image_size);
    }

    int delay = 10000;
    if (char *env_delay = getenv("GTI_DEMO_DELAY"))
        delay = atoi(env_delay);
    waitKey(delay);
    return 0;
}
