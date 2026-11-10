SAMPNAME=XPStoPDF
OTHER_OBJS = $(SAMPNAME).o

# The XPS2PDF plugin ships for Windows and Linux. Windows builds use the
# Visual Studio project, so this Makefile only gates Unix-like hosts: build
# where the plugin is available (values of `uname`); anywhere else, such as
# macOS, `make` and `make clean` are no-ops instead of failing.
# Keep this in sync with the XPStoPDF condition in All/GNUmakefile.
XPS_SUPPORTED_PLATFORMS := Linux
XPS_PLATFORM := $(shell uname)

ifneq ($(filter $(XPS_PLATFORM),$(XPS_SUPPORTED_PLATFORMS)),)

include ../../dlutils/common.mak

$(SAMPNAME).o: $(SRC)/$(SAMPNAME).cpp

else

.PHONY: default clean
default:
	@echo "$(SAMPNAME): skipped, the XPS2PDF plugin is not supported on $(XPS_PLATFORM)."

clean:
	@:

endif
