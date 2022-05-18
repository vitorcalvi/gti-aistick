//***********************************************************************
//
// Copyright (c) 2017-2019 Gyrfalcon Technology Inc. All rights reserved.
// See LICENSE file in the project root for full license information.
//
//***********************************************************************

#include <iostream>
#include <iomanip>
#include <fstream>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <linux/fs.h>

#include <libusb-1.0/libusb.h>

using namespace std;

const int FILESIZE=200;
char patternFile[FILESIZE] = "";
char outputFile[FILESIZE] = "";
char goldenFile[FILESIZE] = "";
int outputSize = 32768;
int vendorId = 12300;
int productId = 22529;
const int chunk_size = 128*1024;
const int max_retry = 20;

uint64_t GetTickCount64(){
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000000ULL + (tv.tv_usec));
}

#if 0
int dump_buffer(const string &filename, char* buf, int size)
{
    std::ofstream fout(filename, ios::out | ios::binary );
    if(!fout.is_open())
    {
        cout<<"Could not open file: "<< filename << endl;
        return -1;
    }
    fout.write(buf, size);
    fout.close();
    return 0;
}
#endif

int nativeUsb_read(libusb_device_handle * dev_handle, unsigned char *dataBuffer, int in_length)
{
    uint8_t      bmRequestType = 0x40;
    uint8_t      bRequest = 0xd0;
    uint16_t     wValue = 0x0000;
    uint16_t     wIndex = 0x000d;

    uint32_t addr;
    unsigned char data[9];

    addr = 0x00008000;
    data[0]= 0x81;
    data[1]= (unsigned char)(addr&0xff);
    data[2]= (unsigned char)((addr>>8)&0xff);
    data[3]= (unsigned char)((addr>>16)&0xff);
    data[4]= (unsigned char)((addr>>24)&0xff);
    data[5]=(unsigned char)(in_length&0xff);
    data[6]=(unsigned char)((in_length>>8)&0xff);
    data[7]=(unsigned char)((in_length>>16)&0xff);
    data[8]=(unsigned char)((in_length>>24)&0xff);

    usleep(2000);

    int r;
    r = libusb_control_transfer( dev_handle, bmRequestType, bRequest, wValue, wIndex, data, 9, 0);
    if(r != 9)  {
        cout << "Control Writing error!" << endl;
        return -1;
    }

    usleep(2000);

    int actual=0;
    r = libusb_bulk_transfer(dev_handle, (1 | LIBUSB_ENDPOINT_IN), dataBuffer, in_length, &actual, 0);
    if(actual != in_length) {
        cout << "USB Bulk input error! read/expected: "<<actual<<","<<in_length << endl;
    }
    return actual;
}


int nativeUsb_write(libusb_device_handle * dev_handle,  unsigned char *dataBuffer, int out_length)
{
    uint8_t      bmRequestType = 0x40;
    uint8_t      bRequest = 0xd0;
    uint16_t     wValue = 0x0000;
    uint16_t     wIndex = 0x000d;

    //out_length = min(516*1024, out_length);

    uint32_t addr;
    unsigned char data[9];
    addr = 0x00008000;
    data[0]= 0x02;
    data[1]= (unsigned char)(addr&0xff);
    data[2]= (unsigned char)((addr>>8)&0xff);
    data[3]= (unsigned char)((addr>>16)&0xff);
    data[4]= (unsigned char)((addr>>24)&0xff);
    data[5]= (unsigned char)(out_length&0xff);
    data[6]= (unsigned char)((out_length>>8)&0xff);
    data[7]= (unsigned char)((out_length>>16)&0xff);
    data[8]= (unsigned char)((out_length>>24)&0xff);

    usleep(2000);

    int r;
    r = libusb_control_transfer(dev_handle, bmRequestType, bRequest, wValue, wIndex, data, 9, 0);
    if(r != 9)  {
        cout << "Control Writing error!" <<endl;
        return -1;
    }

    usleep(2000);

    int actual=0;
    r = libusb_bulk_transfer(dev_handle, (2 | LIBUSB_ENDPOINT_OUT), (unsigned char *)dataBuffer, out_length, &actual, 0);
    if(actual != out_length) {
        cout << "USB Bulk out error! wrote/expected: "<<actual<<","<<out_length << endl;
    }
    return actual;
}

int read_bin_to_file(libusb_device_handle * dev_handle, char *filename, int size)
{
    char * memblock = nullptr;
    ofstream file (filename, ofstream::binary);
    if (! file.is_open()) {
        cout << "cannot open file " << filename << endl;
        return -1;
    }
    memblock = new char [chunk_size];
    int ret = size;
    int retry = 0;
    while (size > 0) {
        size_t len = std::min(chunk_size, size);
        int lenRet = nativeUsb_read(dev_handle, (unsigned char *)memblock, len);
        if (lenRet < 0) {
            cout << "USB Read error, remaining size " << size;
            break;
        }
        else if (lenRet == 0) // not done, retry
            retry ++;
        else 
            retry = 0;
        if (retry > max_retry )
            break;
        file.write((char *)memblock, lenRet);
        size -= lenRet;
    }
    delete[] memblock;
    file.close();
    return (ret-size);
}

int read_bin_to_buffer(libusb_device_handle * dev_handle, char *buffer, int size)
{
    int ret = size;
    int retry = 0;
    while (size > 0) {
        size_t len = std::min(chunk_size, size);
        int lenRet = nativeUsb_read(dev_handle, (unsigned char *)buffer, len);
        if (lenRet < 0) {
            cout << "USB Read error, remaining size " << size;
        }
        else if (lenRet == 0) // not done, retry
            retry ++;
        else 
            retry = 0;
        if (retry > max_retry )
            break;
        buffer += lenRet;
        size -= lenRet;
    }
    return (ret-size);  // return the number of bytes read
}

int write_bin_from_file(libusb_device_handle * dev_handle, char *filename)
{
    streampos spsize;
    char * dataBuffer = nullptr;
    ifstream file (filename, ios::in|ios::binary|ios::ate);
    if (file.is_open()) {
        spsize = file.tellg();
        dataBuffer = new char [spsize];
        //cout << "created buffer "<< hex <<dataBuffer<<dec << endl;
        file.seekg (0, ios::beg);
        file.read (dataBuffer, spsize);
        file.close();
    }
    else {
        cout << "cannot open file " << filename << endl;
        return -1;
    }
    char *buffer = dataBuffer;
    int size = (int)spsize;
    int ret = size;
    int retry = 0;
    while (size > 0) {
        size_t len = std::min(chunk_size, size);  // transfer maximum chunk_size bytes
        int lenRet = nativeUsb_write(dev_handle, (unsigned char*)buffer, len);
        if (lenRet < 0) {
            cout << "USB write error, remaining size " << size;
            break;
        }
        else if (lenRet == 0) // not done, retry
            retry ++;
        else 
            retry = 0;
        if (retry > max_retry )
            break;
        buffer += lenRet;
        size -= lenRet;
    }

    if(dataBuffer)
        delete[] dataBuffer;

    return (ret-size); // return the number of bytes written
}


int buffer_cmp(char *buf1, char *buf2, int size)
{
    if( buf1 == nullptr || buf2 == nullptr )
        return 1; 
    while(size && !(*buf1 - *buf2)) {
        size--;
        buf1++;
        buf2++;
    }
    return (int)(*buf1 - *buf2);
}


int main(int args, char **argv)
{   
    if(args < 3)
    {
        cout << "Usage:" << endl;
        cout << "       ./usbTool patternFileName  goldenFileName  outputFileName" << endl;
        exit(-1);
    }
//    cout << "args :" << args << endl;
    if(argv[1])
        strcpy(patternFile, argv[1]);

    if(argv[2])
        strcpy(goldenFile, argv[2]);

    if(argv[3])
        strcpy(outputFile, argv[3]);

  //  cout << "Arguements: " << patternFile << " | " << goldenFile << " | " << outputFile << endl;


    cout << "USB init - ";
    if(! strcmp(patternFile, "") ) {
        cout << "Missing pattern file"<< endl;
        return 1;
    }

    libusb_device_handle *dev_handle; //a device handle
    libusb_context *ctx = NULL; //a libusb session
    int r;
    r = libusb_init(&ctx); //initialize the library for the session we just declared
    if(r < 0) {
        cout<<"Init Error "<<r<<endl; //there was an error
        return 1;
    }

    dev_handle = libusb_open_device_with_vid_pid(ctx, vendorId, productId);
    if(dev_handle == nullptr)
        cout << "Cannot open device " << hex << vendorId << ":" << productId << dec << endl;
    else
        cout << "Opened device " << hex << vendorId << ":" << productId << dec << endl;

    if(libusb_kernel_driver_active(dev_handle, 0) == 1) { //detach if kernel driver is attached
        cout<<"Kernel Driver Active"<<endl;
        if(libusb_detach_kernel_driver(dev_handle, 0) == 0)
            cout<<"Kernel Driver Detached!"<<endl;
    }
    r = libusb_claim_interface(dev_handle, 0);
    if(r < 0) {
        cout<<"Cannot Claim Interface"<<endl;
        return 1;
    }

    cout << "Loading binary from file " << patternFile << endl;
    if (write_bin_from_file(dev_handle, patternFile) < 0 ) {
        cout << "Failed writing to device" << endl;
    }

    if(strcmp(patternFile, "sleep.bin") == 0)
    {
        cout << "Enter into sleep mode.\n" << endl;

        libusb_release_interface(dev_handle, 0);
        libusb_close(dev_handle);
        libusb_exit(ctx);
        return 0;
    }

    usleep(30*1000);

    streampos goldenSize = 0;
    char * goldenBuffer = nullptr;
    streampos outputfileSize = 0;
    char * outputfileBuffer = nullptr;
    if( outputSize != 0 ) {
        // output size exist
        cout << "Request device to send back " << outputSize << " bytes" << endl;
        if( strcmp(outputFile, "") ) {
            // output filename exist
            cout << "Dump output to file " << outputFile << endl;
            read_bin_to_file(dev_handle, outputFile, outputSize);
            ifstream pfile (outputFile, ios::in|ios::binary|ios::ate);
            if (pfile.is_open()) {
                cout << "Loading output file " << outputFile << endl;
                outputfileSize = pfile.tellg();
                outputfileBuffer = new char [outputfileSize];
                pfile.seekg (0, ios::beg);
                pfile.read (outputfileBuffer, outputfileSize);
                pfile.close();
            }
        }
        else {
            cout << "Reading output data"<< endl;
            outputfileSize = outputSize;
            outputfileBuffer = new char [outputfileSize];
            read_bin_to_buffer(dev_handle, outputfileBuffer, outputfileSize);
        }
    }
    if( strcmp(goldenFile, "") ) {
        cout << "Loading golden data file " << goldenFile << endl;
        ifstream gfile (goldenFile, ios::in|ios::binary|ios::ate);
        if (gfile.is_open()) {
            goldenSize = gfile.tellg();
            goldenBuffer = new char [goldenSize];
            gfile.seekg (0, ios::beg);
            gfile.read (goldenBuffer, goldenSize);
            gfile.close();
        }
    }
    if( outputfileBuffer==nullptr && goldenSize ) {
        cout << "Reading output data " << goldenSize << " bytes" << endl;
        outputfileSize = goldenSize;
        outputfileBuffer = new char [outputfileSize];
        read_bin_to_buffer(dev_handle, outputfileBuffer, goldenSize);
    }

    if( outputfileSize == goldenSize ) {
        if (buffer_cmp(outputfileBuffer, goldenBuffer, goldenSize))
            cout << "Test FAILED"<< endl;
        else
            cout << "Test PASSED"<< endl;
    }
    else {
        cout << "Test FAILED"<< endl;
    }

    if(goldenBuffer!=nullptr)
        delete[] goldenBuffer;
    if(outputfileBuffer!=nullptr)
        delete[] outputfileBuffer;
    libusb_release_interface(dev_handle, 0);
    libusb_close(dev_handle);
    libusb_exit(ctx);

    return 0;
}

