#-------------------------------------------------
#
# Project created by QtCreator 2023-04-14T09:29:22
#
#-------------------------------------------------

QT       -= core gui

TARGET = modbus
TEMPLATE = lib
#CONFIG += staticlib

VERSION = 1.0.0

############################################################
#DEFINES += HW_ARM_RZG2L        #RZG2L硬件
#DEFINES += HW_ARM_TI           #TI335X硬件
#DEFINES += HW_LINUX_X86         #X86 Linux

# 判断硬件平台
unix:{
contains(QT_ARCH, arm64){
    DEFINES += HW_ARM_RZG2L
    message("arm linux 64")
    suffix = lib_arm_rzg2l
}else{
contains(QT_ARCH, arm){
    message("arm linux 32")
    DEFINES += HW_ARM_TI
    suffix = lib_arm_ti
}else{
    message("linux x86")
    DEFINES += HW_LINUX_X86
    suffix = lib_x86_linux
}
}
}
win32:{
    DEFINES += HW_WIN_X86
    suffix = lib_x86_win
}


GIT_BRANCH   = $$system(git rev-parse --abbrev-ref HEAD)
GIT_HASH     = $$system(git show --oneline --format=\"%ci-%cn\" -s HEAD)
GIT_HASH2     = $$system(git describe --tags --dirty)
#PC_VERSION = "$${GIT_BRANCH}-$${GIT_HASH}"
PC_VERSION = "$${GIT_HASH2}"
GIT_BRANCH_ = "$${GIT_BRANCH}"
GIT_HEAD = "$${GIT_HASH}"
DEFINES += GIT_COMMIT_VERSION=\"\\\"$$PC_VERSION\\\"\"
DEFINES += GIT_BRANCH_INFO=\"\\\"$$GIT_BRANCH_\\\"\"
DEFINES += GIT_HASH_INFO=\"\\\"$$GIT_HEAD\\\"\"

message($${GIT_BRANCH})



CONFIG(debug, debug|release) {
suffix = $${suffix}_debug
TARGET=$${TARGET}d
}
else{
suffix = $${suffix}_release
}
DESTDIR = output/libs/$${suffix}
OBJECTS_DIR = temp/temp_$${suffix}
UI_DIR = temp/temp_$${suffix}
MOC_DIR = temp/temp_$${suffix}
RCC_DIR = temp/temp_$${suffix}



# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        src/modbus.cpp \
    src/modbusslave.cpp \
    src/crc.cpp \
   src/modbusmaster.cpp

HEADERS += \
        include/modbus.h \
    include/modbusslave.h \
    src/crc.h \
    include/modbusmaster.h

INCLUDEPATH += include/
unix {
    target.path = /usr/lib
    INSTALLS += target
}


#CONFIG(release, debug|release) {
#contains(DEFINES,HW_ARM_TI){
#unix:QMAKE_POST_LINK += "arm-linux-gnueabihf-strip $${DESTDIR}/$${TARGET} && "
#}
#contains(DEFINES,HW_ARM_RZG2L){
#unix:QMAKE_POST_LINK += "aarch64-poky-linux-strip $${DESTDIR}/$${TARGET} && "
#}
#}

unix:QMAKE_POST_LINK += "mkdir output/include -p && cp include/*.h  output/include/ -r"
