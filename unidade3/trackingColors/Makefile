BASEDIR = ../..

CPP = g++
CXX	=	g++
CXXFLAGS = -Wall -std=c++11	-I `pkg-config --cflags opencv` -I ${BASEDIR}/include
LDFLAGS := -L ${BASEDIR}/lib
LDLIBS :=	-lsystem_tools `pkg-config --libs opencv`

all: trackingColor
