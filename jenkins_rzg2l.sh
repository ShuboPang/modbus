#!/bin/bash
## ${1} sdk所在目录  /home/ubuntu/ti-processor-sdk-linux-rt-am335x-evm-04.00.00.04
## ${2} 工程目录 /home/ubuntu/project/LXController
echo 开始构建RZG2L版本
echo `whoami`

function buildRZG2L(){
    SDK_path=/opt/poky
    #SDK_path=/home/ubuntu/poky
    project_path=.
    unset LD_LIBRARY_PATH
    source ${SDK_path}/3.1.5/environment-setup-aarch64-poky-linux
    cd ${project_path} && rm ${project_path}/output/* -rf &&
    rm Makefile -rf &&
    ${SDK_path}/3.1.5/sysroots/x86_64-pokysdk-linux/usr/bin/qt5/qmake  ${project_path}/modbus.pro -spec linux-oe-g++ && 
    /usr/bin/make clean -j8 && 
    /usr/bin/make qmake_all &&
    make -j8  && 
    output_path=${project_path}/output/ && 
    cp ${output_path}/* ${jenkins_output_path}
}

# 设置需要构建的参数  可选参数

buildRZG2L


