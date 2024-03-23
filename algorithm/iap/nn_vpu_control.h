#ifndef __VPU_CONTROL_H__
#define __VPU_CONTROL_H__

#include <stdint.h>

/******************************* Definition **********************************/
#ifndef VPU_VMEM_BASE
#define VPU_VMEM_BASE       (0xA0000000U)
#endif
#ifndef VPU_CODE_ADDR
#define VPU_CODE_ADDR       (0xA0208000u)
#endif
#ifndef VPU_CODE_SIZE
#define VPU_CODE_SIZE       (520)
#endif
#ifndef VPU_MEM_SIZE
#define VPU_MEM_SIZE        (0x200000)
#endif
#ifndef VPU_EXRA_INFO_SIZE
#define VPU_EXRA_INFO_SIZE  (0x10000)
#endif

// VPU status & control register
#define VPU_CONTROL_REG 	(0x51B00040U)
#define VPU_ERRCODE_OFFSET	(12)		// 4bits
#define VPU_PSRAM_BASE  	(0x80000000U)

#define MAX_VPU_CODE_SIZE	(4096)			// max vpucode size

 
enum VCODE_REG{
	R0 = 0,
	R1,
	R2,
	R3,
	R4,
	R5,
	R6,
	R7,
	R8,
	R9,
	R10,
	R11,
	R12,
	R13,
	R14,
	R15	
};

enum VCODE_EXEC{
	MUL = 0xF000,
	MULC = 0xF100,
	SCR = 0xF200,
	EXP = 0xF300,
	VADD = 0xF400,
	ADDC = 0xF500,
	SUB = 0xF600,
	SUBC = 0xF700,
	DOT = 0xF800,
	DOTC = 0xF900,
	XOR = 0xFA00,
	LUT = 0xFB00,
	VMAX = 0xFC00,
	VMIN = 0xFD00,
	RSTD = 0xFE00,
	FLUSHD = 0xFF00,	
};

#define EXEC(op_code, op_num)	\
		((uint16_t)((op_code) | (op_num)&0xFF))

#define INC(r, data)	\
		((uint16_t)((0x4000) | (r)<<8 | data))

/*#define DEC(r, data)	\
		((uint16_t)((0x4000) | (r)<<8 | data))*/

#define LOADH(r, data) \
		((uint16_t)(0x3000| (r<<8) | ((data)&0xFF00)>>8))

#define LOADL(r, data) \
		((uint16_t)(0x2000| (r<<8) | ((data)&0xFF)))

#define CCFG0(op_h, lut, truncate, q, b8, lpfirst) \
		((uint16_t)(0x8000| ((op_h)&0x100)>>8 | (lpfirst) | (b8)<<1 | (q)<<2 | (truncate)<<3 | (lut)<<7))

#define CCFG1(ARoll, BRoll, CRoll, Dinv, Drelu, Cinv, Crelu, Binv, Brelu, Ainv, Arelu) \
		((uint16_t)((0x9000|ARoll<<11) | (BRoll<<10) | (CRoll<<9) | (Dinv<<7) | (Drelu<<6) | (Cinv<<5) | \
		 (Crelu<<4) | (Binv<<3) | (Brelu<<2) | (Ainv<<1) | Arelu))

#define CCFG2(len, dst) \
        ((uint16_t)(0xa000| (((len)&0x3FF)<<2) | (dst)))

#define CCFG3(addr_h, loop) \
        ((uint16_t)(0xb000| (((addr_h)&0x1FF000)>>5) | (loop)))

#define CCFG4(addr_l) \
        ((uint16_t)(0xc000|((addr_l)&0xFFF)))

#define LOADW(r, lbc, len_h, mode) \
        ((uint16_t)(0xd000| (r)<<8 | (lbc)<<4 | (((len_h)>>10)&0x7)<<1 | (mode)))

#define STOREW(len_h) \
        ((uint16_t)(0xd020|(((len_h)>>10)&0x7)<<1))

#define CCFGF(a, b, c, d, op_hh) \
        ((uint16_t)(0xe000|((op_hh)&0x600)>>9))

#define SET_VCODE_REG(addr, code)	\
        *((volatile uint32_t *)(addr)) = (code)

#define GET_VCODE_REG(addr)	\
        *((volatile uint32_t *)(addr))

#define SAT_INT16T(x) ((x)>32767?32767:((x)<(-32768)?(-32768):(x)))
#define SAT_INT32T(x) ((x)>2147483647?2147483647:((x)<(-2147483648)?(-2147483648):(x)))
#define ALIGN_16(x)   (((x+15)/16)*16)

void VpuStart(unsigned short startAddr);
void load_vcode(int32_t model_index);
void load_vcode_layers(int32_t model_index, int32_t start_id, int32_t end_id);
#endif

