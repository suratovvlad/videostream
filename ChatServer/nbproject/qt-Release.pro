# This file is generated automatically. Do not edit.
# Use project properties -> Build -> Qt -> Expert -> Custom Definitions.
TEMPLATE = app
DESTDIR = dist/Release/MinGW_static-Windows
TARGET = ChatServer
VERSION = 1.0.0
CONFIG -= debug_and_release app_bundle lib_bundle
CONFIG += debug 
PKGCONFIG +=
QT = core gui network
SOURCES += main.cpp server.cpp user.cpp videostream/SampleVideo.cpp videostream/bmp.c videostream/bmp2tmin_ok_18_09_small.c videostream/videoInput.cpp
HEADERS += server.h user.h videostream/bmp.h videostream/videoInput.h
FORMS +=
RESOURCES +=
TRANSLATIONS +=
OBJECTS_DIR = build/Release/MinGW_static-Windows
MOC_DIR = 
RCC_DIR = 
UI_DIR = 
QMAKE_CC = gcc
QMAKE_CXX = g++
DEFINES += 
INCLUDEPATH += 
LIBS += -lgdi32 -ltheora -logg -luser32 -loleaut32 -lole32  
