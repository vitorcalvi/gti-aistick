#!/bin/sh

if [ -z "$1" ]; then
  target="aarch64"
else
  if [ "$1" = "x86_64" ] || [ "$1" = "armv7l" ] || [ "$1" = "armv8" ] || [ "$1" = "aarch64" ] || [ "$1" = "mips" ] ; then
    target="$1"
  else
    target="x86_64"
  fi
fi
echo "Building for ${target} ..."

if [ "${target}" = "armv7l" ]; then
  progprefix="arm-linux-gnueabihf"
elif [ "${target}" = "armv8" ]; then
  progprefix="armv8l-linux-gnueabihf"
elif [ "${target}" = "aarch64" ]; then
  progprefix="aarch64-linux-gnu"
elif [ "${target}" = "mips" ]; then
  progprefix="mipsel-openwrt-linux"
else #x86_64 and others
  echo "cannot find toolchain"
  exit
fi
which ${progprefix}-gcc > /dev/null
if [ $? != 0 ]; then 
  echo "toolchain ${progprefix} not found.  Stop"; 
  echo "Please add toolchain to your path."; 
  exit;
else
  echo "found toolchain ${progprefix}";
fi


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

if [ ! -z ${progprefix} ]; then
./configure --prefix=$(readlink -f "$PWD/../" ) --host="${progprefix}" --disable-udev
else
./configure --prefix=$(readlink -f "$PWD/../" )
fi

make
make install
make clean

cd -
cp -ravf include/* ../../Include/
cp -avf lib/libusb-1.0.a ../../Lib/Linux/${target}/libusb-1.0.a
cp -avf lib/libusb-1.0.so.0.1.0 ../../Lib/Linux/${target}/libusb-1.0.so
touch .stamp_installed

