#
# Solaris makefile for APDFL samples
# Set the values for CC and CXX below to match your environment.
#

ifeq ($(STAGE), debug)
    DEBUG=-DDEBUG -D_DEBUG
else
    DEBUG=-DNDEBUG
endif


ifeq ($(BUILD_64_BIT), true)

CCFLAGS  = -g -m64 $(PDF_FDIR_DEF) $(DEBUG)
LDFLAGS = -g -m64 -R\$${ORIGIN}/$(PDFL_PATH) -L$(PDFL_PATH)

else  # 32-bit w/ gcc

CCFLAGS  = -g -m32 $(PDF_FDIR_DEF) $(DEBUG)
LDFLAGS = -g -m32 -R\$${ORIGIN}/$(PDFL_PATH) -L$(PDFL_PATH)

endif

CCFLAGS += -DPRODUCT=\"HFTLibrary.h\" -DPLATFORM=\"UnixPlatform.h\" -DUNIX_PLATFORM=1 -DUNIX_ENV=1 -D_REENTRANT \
-Wno-multichar -DPDFL_SDK_SAMPLE -DPI_ACROCOLOR_VERSION=AcroColorHFT_VERSION_6 -DTOOLKIT

CXXFLAGS = -std=c++11  $(CCFLAGS)

LD = $(CXX)

LIBS = -lDL180pdfl -lDL180CoolType -lDL180AGM -lDL180BIB -lDL180ACE -lDL180ARE \
	   -lDL180BIBUtils -lDL180JP2K -lDL180AdobeXMP -lDL180AXE8SharedExpat \
	   -licuuc -licudata -lpthread -lsocket -lnsl -lc -lm
