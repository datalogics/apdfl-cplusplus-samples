
FRAMEWORK_PATH=$(PDFL_PATH)

ifeq ($(STAGE), debug)
    DEBUG=-DDEBUG -D_DEBUG -fpascal-strings
else
    DEBUG=-DNDEBUG
endif

CCFLAGS  = -g $(PDF_FDIR_DEF) -DNO_PRAGMA_ONCE -DMAC_PLATFORM=1 -DMAC_ENV=1 -DPRODUCT=\"HFTLibrary.h\" $(DEBUG) -D_REENTRANT -Wno-multichar -DPDFL_SDK_SAMPLE -DPI_ACROCOLOR_VERSION=AcroColorHFT_VERSION_6 -DTOOLKIT

CXXFLAGS = -std=c++11 $(CCFLAGS) -x objective-c++

LDFLAGS = -Wl,-rpath,$(PDFL_PATH) -L$(PDFL_PATH)
LIBS = -F $(FRAMEWORK_PATH) -framework DL180pdfl -framework DL180CoolType -framework DL180AGM -framework DL180BIB -framework DL180ACE -framework DL180ARE -framework Cocoa\
	   -framework DL180BIBUtils -framework DL180JP2K -framework DL180AdobeXMP -framework DL180AXE8SharedExpat \
	   -licuuc -licudata -lpthread
