#-------------------------------------------------
#
# Project created by QtCreator 2014-10-11T19:03:02
#
#-------------------------------------------------

QT       += core gui opengl multimedia
CONFIG   += C++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Surface3DLight
TEMPLATE = app


SOURCES += main.cpp\
    scene3D.cpp \
    camvideosurface.cpp

HEADERS  += scene3D.h \
    camvideosurface.h
