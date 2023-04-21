include(gtest_dependency.pri)

TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG += thread
CONFIG -= qt

INCLUDEPATH += ../include
INCLUDEPATH += ..

HEADERS += \
        ../src/crc.h \
        modbusmastertest.h \
        modbusslavetest.h \
        tst_modbustest.h

SOURCES += \
        ../src/crc.cpp \
        ../src/modbus.cpp \
        ../src/modbusmaster.cpp \
        ../src/modbusslave.cpp \
        main.cpp \
        modbusmastertest.cpp \
        modbusslavetest.cpp
