GXX34 = 1 
CXX = g++
VERSION=
CXXFLAG = -Wall -ggdb3 -O2 
CXXFLAG_R = -Wall -O2 -O3

DBCXXFLAG = $(CXXFLAG) -pthread

LINK_CXXFLAG = $(CXXFLAG) -Wl,-rpath,../../bin
LINK_CXXFLAG_R = $(CXXFLAG_R) -Wl,-rpath,../../bin

MCXXFLAG = -pthread $(LINK_CXXFLAG) $(DEFINE_SEL)

SRC_COMM = ImageHelper.cpp ExifUtils.cpp main.cpp

OBJ_COMM = $(SRC_COMM:.cpp=.o)
OBJ_COMM_M = $(SRC_COMM:.cpp=.mo)

INCLUDE += -I../include/GraphicsMagick -I../include/libexif
LIB += ../bin/libexif.so ../bin/libGraphicsMagick.so ../bin/libGraphicsMagick++.so -lrt

.SUFFIXES: .o .cpp
.cpp.o:
	$(CXX) $(DBCXXFLAG) $(INCLUDE) -c -o $@ $<

.SUFFIXES: .mo .cpp
.cpp.mo:
	$(CXX) $(DBCXXFLAG) -DMOCK $(INCLUDE) -c -o $@ $<

PROGRAM=imageprocessor

all: imageprocessor

Debug: imageprocessor

cleanDebug: imageprocessor
	rm -rf *.o

imageprocessor: $(OBJ_COMM) $(LIB) $(DLIB) 
	$(CXX) -Wl,-rpath ../lib -o ../bin/$@ $(MCXXFLAG) $(INCLUDE) -D__DEBUG $(OBJ_COMM) $(LIB) $(DLIB) 


depend:
	mkdep $(INCLUDE) $(SRC_COMM) $(CXXFLAG)

install:
	install $(PROGRAM) ../bin/

clean: 
	rm -f *.o
	rm -f *.ro
	rm -f $(PROGRAM)
	rm -f ../bin/$(PROGRAM)
