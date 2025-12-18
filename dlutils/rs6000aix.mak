# Change these to match your local environment

EXTRA_LIBS=-ldl

ifeq ($(STAGE), debug)
    DEBUG=-DDEBUG -D_DEBUG
else
    DEBUG=-DNDEBUG
endif

LD = $(CXX)

CPP_DEFINES = $(PDF_FDIR_DEF) -DUNIX_PLATFORM=1 -DUNIX_ENV=1 -DPRODUCT=\"HFTLibrary.h\" -DPLATFORM=\"UnixPlatform.h\" \
 -DNO_PRAGMA_ONCE -DPDFL_SDK_SAMPLE -DPI_ACROCOLOR_VERSION=AcroColorHFT_VERSION_6


CCFLAGS = -g -Wno-multichar -DAIX_GCC_COMPAT $(CPP_DEFINES) -D_ALL_SOURCE -D_POSIX_SOURCE $(DEBUG) -DRS6000AIX -pthread
CXXFLAGS = -g -Wno-multichar -DAIX_GCC_COMPAT $(CPP_DEFINES) -D_ALL_SOURCE -D_POSIX_SOURCE $(DEBUG) -pthread

# -bnoipath tells the loader to strip the absolute or relative path
# information from the shared library name when it is added to the
# loader section of the object file.  This makes it easier for the
# run time loader to find the shared libraries.
# -brtl enables run time linking for the output file.
LDFLAGS = -g -pthread -Wl,-bnoipath -Wl,-brtl

ifeq ($(BUILD_64_BIT), true)
CCFLAGS += -maix64
CXXFLAGS += -maix64
LDFLAGS += -maix64
endif

LIBS = -L$(PDFL_PATH) -lDL180pdfl -lDL180CoolType -lDL180AGM -lDL180BIB -lDL180ACE -lDL180ARE -lDL180BIBUtils -lDL180JP2K -lDL180AdobeXMP -lDL180AXE8SharedExpat -lpthread
