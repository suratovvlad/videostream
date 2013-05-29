# This file is generated automatically. Do not edit.
# Use project properties -> Build -> Qt -> Expert -> Custom Definitions.
TEMPLATE = app
DESTDIR = dist/Release/MinGW_static-Windows
TARGET = ChatClient
VERSION = 1.0.0
CONFIG -= debug_and_release app_bundle lib_bundle
CONFIG += release 
PKGCONFIG +=
QT = core gui network
SOURCES += client.cpp gui/gui_qt.cpp gui/gui_win32api.cpp gui/mainwindow_win32api.cpp main.cpp
HEADERS += client.h gui.h gui/gui_qt.h gui/gui_win32api.h gui/mainwindow_win32api.h gui/widgets.h resources.h
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
LIBS += -lgdi32  
DEFINES -= _UNICODE UNICODE
CONFIG += console
