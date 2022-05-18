#!/bin/sh

VERSION="1.0.22"

if [ -d libusb-${VERSION} ]; then
  rm -rf libusb-${VERSION}
fi
if [ -d include ]; then
  rm -rf include
fi
if [ -d lib ]; then
  rm -rf lib
fi
#git clone --branch v1.0.22 https://github.com/libusb/libusb.git
if [ ! -e libusb-${VERSION}.tar.bz2 ]; then
  if [ -e ../download/libusb-${VERSION}.tar.bz2 ]; then
    cp ../download/libusb-${VERSION}.tar.bz2 libusb-${VERSION}.tar.bz2
  else
    wget https://github.com/libusb/libusb/releases/download/v${VERSION}/libusb-${VERSION}.tar.bz2
  fi
  if [ -e libusb-${VERSION}.tar ]; then 
    rm -f libusb-${VERSION}.tar
  fi
fi
bzip2 -d libusb-${VERSION}.tar.bz2
tar xf libusb-${VERSION}.tar

cd libusb-${VERSION}
for i in $(ls ../patches/*.patch); do patch -p1 < ${i}; done
cd -

cd libusb-${VERSION}/android/jni
chmod +x build.sh
./build.sh
cd -

if [ ! -d "../../Include/libusb-1.0/" ]; then
mkdir ../../Include/libusb-1.0/
fi
if [ ! -e "../../Include/libusb-1.0/libusb.h" ]; then
cp ./libusb-1.0.22/libusb/libusb.h ../../Include/libusb-1.0/
fi

touch .stamp_installed

