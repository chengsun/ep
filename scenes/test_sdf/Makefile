OUTPUT_NAME = test_sdf
OUTPUT_SHARED = 1
INSTALL_EXTRA = $(wildcard data/*)
ROOTDIR = ../..

include $(ROOTDIR)/Makefile.include

LDFLAGS += -L$(ROOTDIR)/demolib -ldemolib

include $(ROOTDIR)/Makefile.rules

