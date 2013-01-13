OUTPUT_NAME = player
ROOTDIR = ..

include $(ROOTDIR)/Makefile.include

CPPFLAGS += `$(SDL_CONFIG) --cflags`
LDFLAGS += `$(SDL_CONFIG) --libs`
ifeq ($(PLATFORM),LINUX)
    LDFLAGS += -ldl
endif
LDFLAGS += -L$(ROOTDIR)/demolib -ldemolib 

include $(ROOTDIR)/Makefile.rules
