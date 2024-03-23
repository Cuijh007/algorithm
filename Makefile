######################################################################## 
# file:		makefile
#=====================================================================
# Description:	
# 		Makefile for Bach-SOC FW.
#=====================================================================
# version:	v0.1
# date:		2018/4/18
# create by:	xjl
######################################################################## 

########################################################################
#
# Pick up the configuration file in make section
#
########################################################################
include ./build/makeconfig

########################################################################
#
# Create a list of codebases to be compiled 
#
########################################################################

TARGETS		=all 
TARGETS_CLN	=arlib_clean
export FW_PROJ_ROOT=$(shell pwd)

########################################################################
#
# Pick up the default build rules 
#
########################################################################

include $(FW_PROJ_ROOT)/build/makerule/$(DEVICE)/make.$(DEVICE).$(TOOL)

########################################################################
#
# Build the CSP and GEN libraries 
#
########################################################################

libs: $(TARGETS)

all: arlib

arlib:
	@$(ECHO) "*********************************************************"
	@$(ECHO) "* Building" $(ARLIB) $(DEVICE)"support package source -> "
	@$(ECHO) "*   $(ARLIB_SRC_DIR)"
	@$(ECHO) "*********************************************************"
	$(MAKE) TOOL=$(TOOL) -C $(ARLIB_SRC_DIR) BACH_CPU=BACH1C
	@$(ECHO) "*********************************************************"
	@$(ECHO) "* Build arlib done"
	@$(ECHO) "* " `date`
	@$(ECHO) "*********************************************************"

release:
	$(MKDIR)  $(FW_PROJ_ROOT)/release
	$(CP) -Rf $(FW_PROJ_ROOT)/algorithm/library $(FW_PROJ_ROOT)/release
	$(MKDIR)  $(FW_PROJ_ROOT)/release/include
	$(CP)     $(FW_PROJ_ROOT)/algorithm/iap/mid_common.h $(FW_PROJ_ROOT)/release/include
	$(CP)     $(FW_PROJ_ROOT)/algorithm/iap/mid_npu.h $(FW_PROJ_ROOT)/release/include
	$(CP)     $(FW_PROJ_ROOT)/algorithm/iap/nn_vpu_control.h $(FW_PROJ_ROOT)/release/include
	$(CP)     $(FW_PROJ_ROOT)/algorithm/isr/iet_math.h $(FW_PROJ_ROOT)/release/include
	$(CP)     $(FW_PROJ_ROOT)/algorithm/isr/iet_feature.h $(FW_PROJ_ROOT)/release/include
	$(CP)     $(FW_PROJ_ROOT)/algorithm/isr/ISR_FiFo.h $(FW_PROJ_ROOT)/release/include
	$(CP)     $(FW_PROJ_ROOT)/algorithm/hpf/*.h $(FW_PROJ_ROOT)/release/include
	@$(ECHO) -n "Algorithm version: git tag " > $(FW_PROJ_ROOT)/release/readme.txt
	git describe --always --tags --dirty >> $(FW_PROJ_ROOT)/release/readme.txt
	git log -1 >> $(FW_PROJ_ROOT)/release/readme.txt
    
########################################################################
#
# Clean the CSP and GEN libraries 
#
########################################################################

clean: $(TARGETS_CLN)

arlib_clean:
	$(ECHO) "Cleaning" $(ARLIB) "support package ->" $(ARLIB_SRC_DIR)
	$(MAKE) TOOL=$(TOOL) -C $(ARLIB_SRC_DIR) clean -s
	$(RM) $(FW_PROJ_ROOT)/algorithm/library/*.a

# delete all targets this Makefile can make and all built libraries
# linked in
realclean: clean
	@$(MAKE) TOOL=$(TOOL) -C $(ARLIB_SRC_DIR) realclean

distclean: realclean unconfig
	@$(RMREC) "*.o"
	@$(RMREC) "*.elf"
	@$(RMREC) "*.exe"
	@$(RMREC) "*~"
	@$(RMREC) "*.map"
	@$(RMREC) "*.lst"
	@$(RMREC) "*.axf"
	@$(RMREC) "*.bin"
	@$(RMREC) "*.dbo"
	@$(RMREC) "*.dla"
	@$(RMREC) "*.dnm"
	@$(RMREC) "*.srec"
	@$(RMREC) ".depend"
	@$(RMREC) "*.wrn"

.PHONY: libs clean realclean distclean
