#//####################################################################### 
# file:		makefile.BACH.gnu
#=====================================================================
# Description:	
#		Make rules for the GNU toolset
#=====================================================================
# version:	
# date:		
# create by:
######################################################################## 
include $(FW_PROJ_ROOT)/build/makerule/common/make.rules.environment

RISCV_PREFIX    =riscv64-unknown-elf
GNUTOOLS	=
OPTIM		= 2

ARCH_IMC=-march=rv32imc -Wa,-march=rv32imc -mabi=ilp32  
ARCH_IM =-march=rv32im  -Wa,-march=rv32im  -mabi=ilp32 

#===================== C compile flag ============================
CFLAGS    		 = -c 
#CFLAGS    		+= -std=c99
CFLAGS    		+= -std=gnu99
CFLAGS			+= -Wall 
CFLAGS			+= $(ARCH_IMC)
CFLAGS			+= -O$(OPTIM) 
CFLAGS			+= -gdwarf-2 
CFLAGS			+= -fno-hosted 
CFLAGS   		+= -ffunction-sections 
CFLAGS			+= -fdata-sections 
CFLAGS                  += -g 
CFLAGS                  += -Wno-unused-parameter 
CFLAGS                  += -Wno-unused-variable
CFLAGS                  += -msmall-data-limit=8

#CFLAGS                 += -I/opt/riscv64-unknown-elf-gcc-8.3.0-2019.08.0-x86_64-linux-ubuntu14/$(RISCV_PREFIX)/include/
#================= note1 =================================
#CFLAGS                 += -nostartfiles
#CFLAGS                 += -Wl,--no-relax 
#CFLAGS                 += -Wl,--gc-sections
#CFLAGS   		+= -mno-sched-prolog 
#CFLAGS    		+= -std=gnu99
#CFLAGS	                += -mapcs-frame 
#CFLAGS	       	        += -msoft-float 
#CFLAGS	                += -mcpu=$(CPU) 
#CFLAGS	                += -mthumb 

#CFLAGS			+= -mthumb-interwork  
#CFLAGS			+= -mno-bit-align 
#CFLAGS			+=  mstructure-size-boundary=8
#CFLAGS			+= -Wpacked
#CFLAGS			+= -Wpadded
#CFLAGS			+= -fpack-struct=0 

#================ note2 ================================
#CFLAGS			+= -fno-builtin
#CFLAGS			+= -fno-strict-aliasing  
#CFLAGS			+= -D PACK_STRUCT_END=__attribute\(\(packed\)\)
#CFLAGS			+= -D ALIGN_STRUCT_END=__attribute\(\(aligned\(4\)\)\)
#CFLAGS			+= -fmessage-length=0 
#CFLAGS			+= -funsigned-char 
#CFLAGS			+= -Wextra 
#CFLAGS			+= -MMD 
#CFLAGS			+= -MP 
#CFLAGS			+= -MF"$(@:%.o=%.d)" 
#CFLAGS			+= -MT"$(@:%.o=%.d)" 

#================ Build Folder Include ========================
CFLAGS   		+= -I$(PRODUCT_INC_DIR) -I$(CHIP_LIB_INC_DIR) -I$(CORE_INC_DIR) -I$(DEVICE_INC_DIR)

#================ Asm compile flag ========================
AFLAGS    	         = -mcpu=$(CPU) 
AFLAGS   	        += -I$(PRODUCT_INC_DIR) -I$(CHIP_LIB_INC_DIR) -I$(CORE_INC_DIR) -I$(DEVICE_INC_DIR) -gdwarf-2 

ifeq ($(REMAP), 1)
CFLAGS += -D__REMAP__=1
AFLAGS += -D__REMAP__=1
endif

ifeq ($(APP), 1)
CFLAGS += -D__APP__=1
AFLAGS += -D__APP__=1
endif

ifeq ($(FAST_LOAD), 1)
CFLAGS += -D__FAST_LOAD__=1
AFLAGS += -D__FAST_LOAD__=1
endif

ifeq ($(CACHE), 1)
CFLAGS += -D__CACHE__
AFLAGS += -D__CACHE__
endif

BACH_CPU ?= BACH2C

ifeq ($(BACH_CPU), BACH2C)
CFLAGS += -D__BACH2C__
endif

ifeq ($(BACH_ADC_32), 1)
CFLAGS += -D__BACH_ADC_32__
endif

ifeq ($(BACH_ADC_X), 1)
CFLAGS += -D__BACH_ADC_X__
endif


ifeq ($(BACH_LP_JUMP), FLASH)
CFLAGS += -D__LP_JUMP__
AFLAGS += -D__LP_JUMP__
else
ifeq ($(BACH_LP_JUMP), RAM)
CFLAGS += -D__LP_JUMP__
AFLAGS += -D__LP_JUMP__
endif
endif

BACH_QSPI_BUF_SIZE ?= 64

ifdef BACH_QSPI_BUF_SIZE
CFLAGS += -D__QSPI_BUF_SIZE__=$(BACH_QSPI_BUF_SIZE)
endif

BACH_DEBUG_UART ?= 0

ifdef BACH_DEBUG_UART
CFLAGS += -D__BACH_DEBUG_UART__=$(BACH_DEBUG_UART)
endif

BACH_SYSTEM_CLOCK ?= 50000000

ifdef BACH_SYSTEM_CLOCK
CFLAGS += -D__BACH_SYSTEM_CLOCK__=$(BACH_SYSTEM_CLOCK)
AFLAGS += -D__BACH_SYSTEM_CLOCK__=$(BACH_SYSTEM_CLOCK)
endif

BACH_IIS_TYPE ?= INT

ifdef BACH_IIS_TYPE
CFLAGS += -D__BACH_IIS_TYPE__=$(BACH_IIS_TYPE)
AFLAGS += -D__BACH_IIS_TYPE__=$(BACH_IIS_TYPE)
endif

ifdef RELEASE_BRANCH
CFLAGS += -D__LIB_VERSION__=\"$(shell git branch  -vv | grep \* | cut -d ' ' -f2)-$(shell git log --oneline -1 | cut -d ' ' -f1)\"
else
CFLAGS += -D__LIB_VERSION__=\"$(shell git describe --always --tags --dirty)\"
endif

CFLAGS += -D$(LIB_TYPE)


CC       		= $(RISCV_PREFIX)-gcc
CCP      		= $(RISCV_PREFIX)-g++
AS       		= $(RISCV_PREFIX)-as
AR       		= $(RISCV_PREFIX)-ar -r
LD       		= $(RISCV_PREFIX)-gcc
NM       		= $(RISCV_PREFIX)-nm
OBJDUMP  		= $(RISCV_PREFIX)-objdump
OBJCOPY  		= $(RISCV_PREFIX)-objcopy
READELF  		= $(RISCV_PREFIX)-readelf
CODESIZE 		= $(RISCV_PREFIX)-size

#================ LD flag ========================
LK   		         =-static
LK      		+=$(ARCH_IMC) 
LK       		+=-Wl,--start-group 
LK       		+=-lc -lnosys -lg -lstdc++ -lsupc++ --specs=nosys.specs --specs=nano.specs 
LK   			+=-lgcc -lm 
LK       		+=-Wl,--end-group 
LK       		+=-Wl,--gc-sections
LK      		+=-Wl,--no-relax 

LDFLAGS 		=$(ARCH_IMC) 
LDFLAGS 		+=-Wl,--no-relax 

#================ LD note 1 ========================
#LDFLAGS  		 =-nostartfiles 
#LDFLAGS 		+=-nodefaultlibs 
#LDFLAGS 		+=-nostdlib 
#LK       		+=-Wl,--start-group $(TARGET_CHIP_LIB_LIB) 
#LK       		+=-u _printf_float

MAP      		 =-Xlinker -Map -Xlinker
LDESC    		 =-Xlinker -T 
ENTRY    		 =-e
BIN      		 =.bin
EXT      		 =.elf
LEXT     		 = 
REC      		 =.srec
HEX                      =.hex

