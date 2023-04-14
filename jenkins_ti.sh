#!/bin/bash
## ${1} sdk所在目录  /home/ubuntu/ti-processor-sdk-linux-rt-am335x-evm-04.00.00.04
## ${2} 工程目录 /home/ubuntu/project/LXController
echo 开始构建TI版本
echo `whoami`

function buildTI(){
    SDK_path=/opt/ti-processor-sdk-linux-rt-am335x-evm-04.00.00.04
    if [ ! -d ${SDK_path} ];then
        SDK_path=/var/lib/jenkins/ti-processor-sdk-linux-rt-am335x-evm-04.00.00.04
        if [ ! -d ${SDK_path} ];then
            SDK_path=/home/ubuntu/ti-processor-sdk-linux-rt-am335x-evm-04.00.00.04
        fi
    fi
    #SDK_path=/home/ubuntu/ti-processor-sdk-linux-rt-am335x-evm-04.00.00.04
    project_path=.
    source ${SDK_path}/linux-devkit/environment-setup && 
    cd ${project_path}  && rm ${project_path}/output/* -rf  && 
    ${SDK_path}/linux-devkit/sysroots/x86_64-arago-linux/usr/bin/qt5/qmake ${project_path}/modbus.pro -spec linux-oe-g++ && /usr/bin/make qmake_all && make -j8 && cd ${project_path} && 
    output_path=${project_path}/output/ && 
    cp ${output_path}/* ${jenkins_output_path}
}

# 设置需要构建的参数  可选参数

buildTI
