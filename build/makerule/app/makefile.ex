######################################################################## 
# file:		makefile.ex
#=====================================================================
# Description:	
# 		App Makefile 
# Notes:
#
#=====================================================================
# version:	        v1.0
# date:		
# create by:	
######################################################################## 

########################################################################
#
# Pick up the configuration file in make section
#
########################################################################
#include ../../makeconfig

########################################################################
#
# Pick up the default build rules 
#
########################################################################

include $(FW_PROJ_ROOT)/build/makerule/$(DEVICE)/make.$(DEVICE).$(TOOL)

########################################################################
#
# Pick up the assembler and C source files in the directory
# Included core files  
#
########################################################################
include $(FW_PROJ_ROOT)/build/makerule/common/make.rules.ftypes

ADDOBJSS  += $(FW_PROJ_ROOT)/core/Device/$(MANUFACTURE)/$(DEVICE)/source/GCC/startup_$(DEVICE).o  
ADDOBJS   += $(FW_PROJ_ROOT)/core/Device/$(MANUFACTURE)/$(DEVICE)/source/system_$(DEVICE).o  


########################################################################
#
# For some Flag defined in header file in Example directory  
#
########################################################################
CFLAGS += -I$(EXDIRINC) $(TMPPATH) 

# Define __BUILD_WITH_EXAMPLE__ symbol in this case (build app mode)
CFLAGS += -D__BUILD_WITH_EXAMPLE__=1 
CFLAGS += -DVERSION_RISCV=1 


########################################################################
#
# Object file in FW library directory  
#
########################################################################
#x FWSRC     = $(wildcard $(FW_PROJ_ROOT)/$(FWLIB)/source/*.c)
#x FWSRCC    = $(wildcard $(FW_PROJ_ROOT)/$(FWLIB)/source/*.cpp)
#x FWASM     = $(wildcard $(FW_PROJ_ROOT)/$(FWLIB)/source/*.$(ASM_EXT))
#x FWOBJS    = $(FWSRC:%.c=%.o) $(FWSRCC:%.c=%.o) $(FWASM:%.$(ASM_EXT)=%.o)


########################################################################
#
# GNU compiler/linker specific stuff
#
########################################################################
ifeq ($(TOOL), gnu)

MEXT       =.map
MAPFILE    =$(EXECNAME)
AFLAGS 	  +=-gdwarf-2

# Define Required Linker script file in each build mode
ifeq ($(BACH_CPU), BACH2C)
#
LDSCRIPTIROM = $(FW_PROJ_ROOT)/build/makerule/app/gcc_riscv_ram_bach2c.ld

#for bootloader
#LDSCRIPTIROM = $(FW_PROJ_ROOT)/build/makerule/app/gcc_riscv_flash_bach2c.ld
endif

LDSCRIPTIRAM = $(FW_PROJ_ROOT)/build/makerule/app/ldscript_ram_$(TOOL).ld

# Following used for Chip Revision V00 only 
LDSCRIPTIROM_V00 = $(FW_PROJ_ROOT)/build/makerule/app/ldscript_rom_v00_$(TOOL).ld
LDSCRIPTIRAM_V00 = $(FW_PROJ_ROOT)/build/makerule/app/ldscript_ram_v00_$(TOOL).ld

ifeq ($(REMAP), 1)
ifeq ($(BACH_CPU), BACH2C)
#LDSCRIPTIROM = $(FW_PROJ_ROOT)/build/makerule/app/gcc_riscv_ram_bach2c.ld
ifeq ($(BACH_LP_JUMP), FLASH)
#LDSCRIPTIROM = $(FW_PROJ_ROOT)/build/makerule/app/gcc_riscv_sgl_jump_flash.ld
endif
else
LDSCRIPTIROM = $(FW_PROJ_ROOT)/build/makerule/app/gcc_riscv_ram.ld
endif
endif

endif

########################################################################
#
# compiler/linker specific stuff
#
########################################################################
ifeq ($(TOOL), ads)
MEXT        =.map
MAPFILE     =$(EXECNAME)
CFLAGS      +=-g
AFLAGS      +=-g
LDSCRIPT    =../common/ldscript_iram_ads.ld
ADDOBJS     += pad.o
endif
ifeq ($(TOOL), rvw)
MEXT        =.map
MAPFILE     =$(EXECNAME)
CFLAGS      +=-g
AFLAGS      +=-g
LDSCRIPT    =../common/ldscript_iram_ads.ld
ADDOBJS     += pad.o
endif
ifeq ($(TOOL), keil)
MEXT        =.map
MAPFILE     =$(EXECNAME)
CFLAGS      +=-g
AFLAGS      +=-g
LDSCRIPT    =../common/ldscript_iram_ads.ld
ADDOBJS     += pad.o
endif

########################################################################
#
# compiler/linker specific stuff
#
########################################################################

ifeq ($(TOOL), iar)
MEXT        =.map
MAPFILE     =$(EXECNAME)
CFLAGS      +=
AFLAGS      +=
LDESC       = --config
LDSCRIPT    =../common/ldscript_iram_$(TOOL).ld
ADDOBJS     += 
endif

########################################################################
#
# Rules to build the executable 
#
########################################################################

default: rom

ifeq ($(TOOL), gnu)

ifeq ($(REMAP), 1)
rom: CFLAGS += -D__REMAP__=1
endif

rom: LDSCRIPT=$(LDSCRIPTIROM)
rom: AFLAGS += --defsym RAM_MODE=0 
rom: CFLAGS += -D__RAM_MODE__=0
ifeq ($(USEMWLIB), 1)
ifeq ($(USENRPROC), 1)
rom: FWLIBS = -lmiddlewareBACHgnu -L$(FW_PROJ_ROOT)/$(MWLIB)/library -ldriversBACHgnu -L$(FW_PROJ_ROOT)/$(FWLIB)/library -larm3nrproc -L$(FW_PROJ_ROOT)/$(MWLIB)/nrproc
else
rom: FWLIBS = -lmiddlewareBACHgnu -L$(FW_PROJ_ROOT)/$(MWLIB)/library -ldriversBACHgnu -L$(FW_PROJ_ROOT)/$(FWLIB)/library
endif
else
ifeq ($(USENRPROC), 1)
rom: FWLIBS = -ldriversBACHgnu -L$(FW_PROJ_ROOT)/$(FWLIB)/library -larm3nrproc -L$(FW_PROJ_ROOT)/$(MWLIB)/nrproc
else
#rom: FWLIBS = -ldriversBACHgnu -L$(FW_PROJ_ROOT)/$(BSP_LIB)/library -lchipBACHgnu -L$(FW_PROJ_ROOT)/$(CHIP_LIB)/library -lmiddlewareBACHgnu -L$(FW_PROJ_ROOT)/middleware/library
rom: FWLIBS = -lchipBACHgnu -L$(FW_PROJ_ROOT)/$(CHIP_LIB)/library -lmiddlewareBACHgnu -L$(FW_PROJ_ROOT)/middleware/library
endif
endif
#rom: clean_rom debug_status  $(OBJS) $(ADDOBJS) $(ADDOBJSS) $(FWOBJS) 
	#$(LD) $(OBJS) $(ADDOBJS) $(ADDOBJSS) $(FWOBJS) $(LDFLAGS) $(LK) $(SCAN) $(MAP) 
rom:  debug_status  $(OBJS) $(ADDOBJS) $(ADDOBJSS)
	@echo "Linking..."
	@echo "OBJS = " $(OBJS)
	$(LD) $(ADDOBJS) $(OBJS) $(FWLIBS) $(ADDOBJSS) $(LDFLAGS) $(LK) $(SCAN) $(MAP) $(MAPFILE)$(MEXT) $(LDESC) $(LDSCRIPT) -o $(EXECNAME)$(EXT) 
	$(ELFTOHEX) $(EXECNAME)$(EXT) $(EXECNAME)_fw_$(FW_MAJOR)_$(FW_MINOR)$(HEX)
	$(ELFTOREC) $(EXECNAME)$(EXT) $(EXECNAME)$(REC)
	$(ELFTOBIN) $(EXECNAME)$(EXT) $(EFLTBINOPT) $(EXECNAME)_fw_$(FW_MAJOR)_$(FW_MINOR).bin

	$(OBJDUMP) -dx --source $(EXECNAME)$(EXT) > $(EXECNAME).lst
	$(OBJCOPY) -O verilog $(EXECNAME)$(EXT) $(EXECNAME).rom

	$(OBJDUMP)  -D $(EXECNAME)$(EXT) > $(EXECNAME).asm
	$(MKDIR) GCC/Flash
	$(MV) $(MAPFILE)$(MEXT) GCC/Flash/
	$(MV) $(EXECNAME)$(EXT) GCC/Flash/
	$(MV) $(EXECNAME)_fw_$(FW_MAJOR)_$(FW_MINOR)$(HEX) GCC/Flash/
	$(MV) $(EXECNAME)$(REC) GCC/Flash/
	$(CP) $(EXECNAME)_fw_$(FW_MAJOR)_$(FW_MINOR)$(BIN) GCC/Flash/
	$(CODESIZE) GCC/Flash/$(EXECNAME)$(EXT)

ram: LDSCRIPT=$(LDSCRIPTIRAM) 
ram: AFLAGS += --defsym RAM_MODE=1
ram: CFLAGS += -D__RAM_MODE__=1 
ram: clean_ram debug_status $(OBJS) $(ADDOBJS) $(ADDOBJSS) $(FWOBJS) 
	$(LD) $(OBJS) $(ADDOBJS) $(ADDOBJSS) $(FWOBJS) $(LDFLAGS) $(LK) $(SCAN) $(MAP) \
	$(MAPFILE)$(MEXT) $(LDESC) $(LDSCRIPT) -o $(EXECNAME)$(EXT)
	$(ELFTOHEX) $(EXECNAME)$(EXT) $(EXECNAME)$(HEX)
	$(ELFTOREC) $(EXECNAME)$(EXT) $(EXECNAME)$(REC)
	$(ELFTOBIN) $(EXECNAME)$(EXT) $(EFLTBINOPT) $(EXECNAME).bin
	$(MKDIR) GCC/Ram
	$(MV) $(MAPFILE)$(MEXT) GCC/Ram/$(MAPFILE)$(MEXT)
	$(MV) $(EXECNAME)$(EXT) GCC/Ram/$(EXECNAME)$(EXT)
	$(MV) $(EXECNAME)$(HEX) GCC/Ram/$(EXECNAME)$(HEX)
	$(MV) $(EXECNAME)$(REC) GCC/Ram/$(EXECNAME)$(REC)
	$(CODESIZE) GCC/Ram/$(EXECNAME)$(EXT)
	
endif

# Print DEBUG MODE Status
debug_status:
	@$(ECHO) "DEBUG MODE Status -->" $(DEBUG_MODE)

clean_objs: #realclean lpc_clean
	@$(RM) $(ADDOBJS)
	@$(RM) $(ADDOBJSS)
	
clean_ram: clean_objs
	@$(RMDIR) "GCC/Ram"	

clean_rom: clean_objs
	@$(RMDIR) "GCC/Flash"
	
cleanall: clean_ram clean_rom
	@$(RMDIR) "GCC"	

########################################################################
#
# Pick up the compiler and assembler rules
#
########################################################################

include $(FW_PROJ_ROOT)/build/makerule/common/make.rules.build

.PHONY: debug_status 
