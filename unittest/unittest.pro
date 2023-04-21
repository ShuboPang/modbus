include(gtest_dependency.pri)

TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG += thread
CONFIG -= qt

HEADERS += \
        tst_modbustest.h

SOURCES += \
        main.cpp
