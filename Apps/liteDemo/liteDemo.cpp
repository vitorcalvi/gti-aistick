/***********************************************************************
*
* Copyright (c) 2017-2019 Gyrfalcon Technology Inc. All rights reserved.
* See LICENSE file in the project root for full license information.
*
************************************************************************/

#include "GTILib.h"
#include "GtiLog.h"
#include <cstring>
#include <fstream>
#include <chrono>
#include <cmath>

//#define CHECK_RESULTS_MNET_TOMATO
#define PRINT_LABEL

using namespace std;

static unique_ptr<char[]> file_to_buffer(char *filename, int *sizeptr) {
    std::ifstream fin(filename, ios::in | ios::binary);
    if (!fin.is_open()) {
        cout << "Could not open file: " << filename << endl;
        exit(-1);
    }

    fin.seekg(0, std::ios::end);
    *sizeptr = fin.tellg();
    fin.seekg(0, std::ios::beg);
    unique_ptr<char[]> buffer(new char[*sizeptr]);

    fin.read((char *)buffer.get(), *sizeptr);
    fin.close();
    return move(buffer);
}

static void dump_buffer(const string &filename, char *buf, int size) {
    std::ofstream fout(filename, ios::out | ios::binary);
    if (!fout.is_open()) {
        cout << "Could not open file: " << filename << endl;
        exit(-1);
    }
    fout.write(buf, size);
    fout.close();
}

template <class T> T hash(unsigned char *p, unsigned int size) {
    T ret = 0;
    while (size-- > 0)
        ret += (T)(31 * (*p++));
    return ret;
}

#ifdef PRINT_LABEL
void print_label( char* buffer, const char * label_tag )
{
    char label[101] = {0,};
    const char *p = std::strstr(buffer + 20, label_tag);
    p = std::strchr((const char *)p + 7, ' ');
    for (int k = 0; k < 100; k++) {
        if (*p == '\"') {
            label[k] = 0;
            break;
        }
        label[k] = *p++;
    }
    cout << "RESULT:" << label << endl;
}
#endif

int main(int argc, char **argv) {
    int loops = 0;
    bool show_per_loop_perf = true;
    bool check_per_loop_correctness = true;

    if (argc < 3) {
        cout << "usage: " << argv[0] << " GTI_model_file image_file [loops]" << endl;
        cout << "         loops: number of inferences to run"<<endl;
        cout << "   Ex: " << argv[0] << " ../Models/.../gti_gnet3.model ../Data/Image_lite/bridge_c20.bin" << endl;
        cout << "       " << argv[0] << " ../Models/.../gti_mnet.model ../Data/Image_lite/swimming_c40.jpg 10" << endl;

        exit(-1);
    }
    if (argc >= 4) {
        loops = atoi(argv[3]);
    } else {
        loops = 1;
    }

    // Read model data from file
    int modelsize = 0;
    unique_ptr<char[]> modelfile = file_to_buffer(argv[1], &modelsize);

    // get infor from model file
    GtiModelInfor modelInfor;
    if(! GtiGetInforFromModelBuffer((void *)modelfile.get(), modelsize, &modelInfor) ) {
    //if(! GtiGetInforFromModelFile(argv[1], &modelInfor) ) {
        cout << "cannot get model infor" << endl;
        return 0;
    } else {
        cout << "name: " << modelInfor.modelProp.name << endl << "chipName: " << modelInfor.modelProp.chipName << endl;
        cout << "Input  " << "width:"<< modelInfor.inputInfor.width << " height:" << modelInfor.inputInfor.height << " depth:" << modelInfor.inputInfor.depth << " color format:" << modelInfor.inputInfor.colorFormat << endl;
        cout << "Output " << "width:"<< modelInfor.outputInfor.width << " height:" << modelInfor.outputInfor.height << " depth:" << modelInfor.outputInfor.depth << " data format:" << modelInfor.outputInfor.format << endl;
    }

    // Load GTI image classification model
    GtiModel *model = GtiCreateModelFromBuffer((void *)modelfile.get(), modelsize);
    // GtiModel *model=GtiCreateModel(argv[1]);
    if (model == nullptr) {
        cout << "Could not create model: " << argv[1] << endl;
        return (-1);
    }

    // Read BGR plannar format image data from file
    GtiTensor tensor;
    int datasize = 0;
    unique_ptr<char[]> datafile = file_to_buffer(argv[2], &datasize);
    tensor.height = 1;
    tensor.width = 1;
    tensor.depth = datasize;
    tensor.buffer = datafile.get();


    float total_inferencetime = 0.0;

    for (int j = 0; j < loops; j++) {
        // Get the inference results from chip
        auto t0=chrono::high_resolution_clock::now();
        GtiTensor *tensorOut = GtiEvaluate(model, &tensor);
        auto t1=chrono::high_resolution_clock::now();
        if (tensorOut == 0) {
            LOG(ERROR) << "evaluate error,exit";
            break;
        }
        auto duration=chrono::duration_cast<chrono::duration<float>>(t1-t0);
        float inferenceTime = 1000*duration.count()+ numeric_limits<float>::epsilon();
        total_inferencetime += inferenceTime;
        float fps = (1000.0) / inferenceTime;

        // print out various results when not measuring performance
        if(check_per_loop_correctness) {
#ifdef CHECK_RESULTS_MNET_TOMATO
            unsigned int v = ::hash<unsigned int>((unsigned char *)tensorOut->buffer, tensorOut->size);
            if (v !=
                957714) // For "./liteDemo ../Models/2803/gti_mnet_fc40_2803.model ../Data/Image_lite/Tomato.bin" command
            {
                cout << "output hash:" << v << endl;
                cout << "Checksum error, FAILED.\n";
                // exit(-1);
            } else {
                cout << "output hash:" << v << endl;
                cout << "PASSED" << endl;
            }
#endif
        }
        if(show_per_loop_perf) {
            cout << (char *)tensorOut->buffer << endl; //do not remove, jenkins checks this line
            if (loops != 1) cout << j+1 << ": ";
#ifdef PRINT_LABEL
            print_label( (char *)tensorOut->buffer, (const char *)"label" );
#else
            cout << "Output size: " << tensorOut->size << " bytes, output format: " << tensorOut->format << endl;
#endif
            cout << "    Image inference time = " << inferenceTime << " ms, FPS = " << fps << endl;
        }
    }

    if(loops > 1) {
        cout << "=========" << endl;
        float average_inferenceTime=0;
        average_inferenceTime = total_inferencetime / (float)loops ;
        float average_fps = 1000.0 / average_inferenceTime;
        cout << "Completed " << loops << " inferences, average inference time = " << average_inferenceTime << " ms, FPS = " << average_fps << endl;
    }

    GtiDestroyModel(model);
    return 0;
}

