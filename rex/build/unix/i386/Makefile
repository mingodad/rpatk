ROOT_DIR = ../../../..
include $(ROOT_DIR)/build/unix/config.mk

INCLUDE = -I$(ROOT_DIR)/arch/unix/$(ARCHDIR)
OUTDIR = out
MACH = -m32

CFLAGS := $(MACH)
ifeq ($(BLDCFG), release)
CFLAGS += -fPIC -O2
else
CFLAGS += -fPIC -O0 -g -Wall
endif

ifeq ($(CCBLD), yes)
CFLAGS += -fprofile-arcs -ftest-coverage
endif

CFLAGS += $(INCLUDE)
LDFLAGS := $(MACH)

include ../rex.mk
