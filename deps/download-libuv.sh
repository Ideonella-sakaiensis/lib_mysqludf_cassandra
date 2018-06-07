#!/bin/bash
VER=$1
URL="https://dist.libuv.org/dist/v${VER}/libuv-v${VER}.tar.gz"
MODULE_NAME=libuv
echo "Downloading $URL"
wget $URL -O /tmp/${MODULE_NAME}.tar.gz
tar zxvf /tmp/${MODULE_NAME}.tar.gz
rm -rf ${MODULE_NAME}
mv ${MODULE_NAME}-v${VER} ${MODULE_NAME}
