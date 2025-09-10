# DLADD MattK 10June2014: these samples are not designed
# to be built with a multi-process make invocation.
.NOTPARALLEL:	dummy

ifeq ($(OS), )
  PLATFORM = $(shell uname)
  MACHINE = $(shell uname -m)
  BUILD_64_BIT=true
  ifeq ($(PLATFORM), Linux)
    ifeq ($(MACHINE), x86_64)
      OS=i80386linux
    endif
    ifeq ($(MACHINE), aarch64)
      OS=armv8linux
    endif
  endif
  ifeq ($(PLATFORM), Darwin)
    ifeq ($(MACHINE), x86_64)
      OS=mac-x86-64
    endif
    ifeq ($(MACHINE), arm64)
      OS=armv8mac
    endif
  endif
  ifeq ($(PLATFORM), AIX)
    OS=rs6000aix
  endif
  ifeq ($(PLATFORM), SunOS)
    OS=sparcsolaris
  endif
endif

ifeq ($(OS), )
$(error Need to set the OS environment variable)
endif

UTIL = ../../dlutils
# 26Aug2009 - Give each sample a copy of the common modules
# since plugin samples compile these differently
COMMON_OBJS = PDFLInitCommon.o PDFLInitHFT.o InitializeLibrary.o APDFLDoc.o

INCLUDE = ../../../Include/Headers
SOURCE=../../../Include/Source
COMMON=../../_Common

include $(UTIL)/$(subst _64,,$(OS)).mak
include ../../All/paths.rel

default: $(SAMPNAME)

CPPFLAGS = -I. -I$(INCLUDE) -I$(UTIL) -I$(COMMON)
CFLAGS = $(CCFLAGS)

$(SAMPNAME) : $(COMMON_OBJS) $(OTHER_OBJS)
	$(CXX) -o $@ $(COMMON_OBJS) $(OTHER_OBJS) $(LDFLAGS) $(LIBS) $(EXTRA_LIBS)

##
# The files have the source and object in different directories, explicit rules
# are needed. 
#
# And, the '.c' files have to be compiled using the C++ compiler.
##

PDFLInitCommon.o : $(SOURCE)/PDFLInitCommon.c
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

PDFLInitHFT.o : $(SOURCE)/PDFLInitHFT.c
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

InitializeLibrary.o : $(COMMON)/InitializeLibrary.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

APDFLDoc.o : $(COMMON)/APDFLDoc.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

TextExtract.o : $(COMMON)/TextExtract.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	$(RM) *.o $(UTIL)/*.o core out.* $(SAMPNAME) 

