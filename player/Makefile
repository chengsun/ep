OUTPUT_NAME = player
ROOTDIR = ..

include $(ROOTDIR)/Makefile.include

ifeq ($(PLATFORM),LINUX)
    LDFLAGS += -ldl
endif
LDFLAGS += -L$(ROOTDIR)/demolib -ldemolib 

include $(ROOTDIR)/Makefile.rules
