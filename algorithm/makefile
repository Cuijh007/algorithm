######################################################################## 
#
# file:		makefile
#=====================================================================
# Description:	
# 		Makefile for device driver.
#=====================================================================
# version:	v1.0
# date:		12 Nov 2014
# create by:	xjl
######################################################################## 

########################################################################
#
# Pick up the configuration file in make section
#
########################################################################
include $(FW_PROJ_ROOT)/build/makeconfig 

########################################################################
#
# Pick up the default build rules 
#
########################################################################

include $(FW_PROJ_ROOT)/build/makerule/$(DEVICE)/make.$(DEVICE).$(TOOL)

########################################################################
#
# Pick up the assembler and C source files in the directory  
#
########################################################################

ASM_EXT  =

ifeq ($(TOOL), gnu)
ASM_EXT	=S
endif

SRC += $(wildcard $(FW_PROJ_ROOT)/algorithm/iap/*.c)
SRC += $(wildcard $(FW_PROJ_ROOT)/algorithm/isr/*.c)
#SRC += $(wildcard $(FW_PROJ_ROOT)/algorithm/hpf/*.c)
SRC += $(wildcard $(FW_PROJ_ROOT)/algorithm/post_process/*.c)

SRCC = $(wildcard *.cpp)
OBJS = $(SRC:%.c=%.o) $(SRCC:%.cpp=%.o) $(ASM:%.$(ASM_EXT)=%.o)

AFLAGS +=-I.
CFLAGS +=-I. -I./denoise -I./iap -I./isr -I./hpf -I./post_process

CFLAGS += -I$(FW_PROJ_ROOT)/driver/core/Device/INTEN/BACH/include
CFLAGS += -I$(FW_PROJ_ROOT)/driver/core/RISCV
CFLAGS += -I$(FW_PROJ_ROOT)/driver/chip/include
CFLAGS += -I$(FW_PROJ_ROOT)/driver/board/include

########################################################################
#
# Build the library
#
########################################################################

$(TARGET_ARLIB_LIB) : .vias $(OBJS) $(ARLIB_LIB_DIR) $(ADDOBJSS) $(ADDOBJS)
	@$(ECHO) "creating" $(ARLIB) "Middleware support package library" 
	$(AR) $@ $(OBJS) $(ADDOBJSS) $(ADDOBJS)

$(ARLIB_LIB_DIR):
	$(MKDIR) $(ARLIB_LIB_DIR)	

# delete all targets this Makefile can make
lib_clean:
	-@$(RM) $(OBJS)

# delete all targets this Makefile can make and all built libraries
# linked in
lib_realclean: 
	-@$(RM) $(ARLIB_LIB_DIR)/*.a
	-@$(RMDIR) $(ARLIB_LIB_DIR)
	
clean: lib_clean
realclean: lib_realclean

########################################################################
#
# Compile the code base 
#
########################################################################

include $(FW_PROJ_ROOT)/build/makerule/common/make.rules.build

.PHONY: all lib_clean lib_realclean
