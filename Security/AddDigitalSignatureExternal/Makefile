SAMPNAME=AddDigitalSignatureExternal
OTHER_OBJS = $(SAMPNAME).o

# The sample's stand-in key service signs with OpenSSL's libcrypto. Set OPENSSL_DIR to an
# OpenSSL 3 or later installation, the directory that holds include/ and lib/, for example:
#     make OPENSSL_DIR=/opt/homebrew/opt/openssl@3
ifeq ($(OPENSSL_DIR), )
$(error Set OPENSSL_DIR to an OpenSSL installation, the directory that holds include/ and lib/)
endif

EXTRA_LIBS = -L$(OPENSSL_DIR)/lib -lcrypto

include ../../dlutils/common.mak

CPPFLAGS += -I$(OPENSSL_DIR)/include

$(SAMPNAME).o: $(SRC)/$(SAMPNAME).cpp
