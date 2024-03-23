######################################################################## 
# file:		makefile
#=====================================================================
# Description:	
# 		Makefile for RTX compilation.
# Notes:
#
#=====================================================================
# version:	v1.0
# date:		2018/4/17
# create by:	xjl
######################################################################## 

EXECNAME    =vpu_test

########################################################################
#
# Pick up the configuration file in make section
#
########################################################################
include ../../build/makeconfig 
EXDIRINC	=$(CURDIR)
include ../../build/makerule/app/makefile.ex
# DO NOT DELETE

clean:
	rm -rf *.o *.lst *.wrn *.depend  *.rom result test_decoder *.bin *.lst *.ld
	rm -rf GCC
