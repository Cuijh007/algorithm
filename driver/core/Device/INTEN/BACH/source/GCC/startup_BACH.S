// Copyright 2019 Intenginetech

//#define  __BOOT_LOADER__

.section .text 
.global reset_handler

/* ============================ [ exceptions ] -========================= */
/* This section has to be down here, since we have to disable rvc for it  */
.section .vectors, "ax"
.option norvc;
.global  __vectors__

// external interrupts are handled by the same callback
// until compiler supports IRQ routines
.org 0x00
#define EXCEPTION_STACK_SIZE 0x58
    jal x0, DMAC_IRQ_Handler         // 0
    jal x0, DMAC_COMMON_IRQ_Handler  // 1       
    jal x0, DMAC_CH0_IRQ_Handler     // 2    
    jal x0, DMAC_CH1_IRQ_Handler     // 3           
    jal x0, DMAC_CH2_IRQ_Handler     // 4
    jal x0, DMAC_CH3_IRQ_Handler     // 5
    jal x0, VPU_IRQ_Handler          // 6
    jal x0, HIFI_CH0_IRQ_Handler     // 7 
    jal x0, HIFI_CH1_IRQ_Handler     // 8
    jal x0, HIFI_CH2_IRQ_Handler     // 9
    jal x0, HIFI_CH3_IRQ_Handler     // 10 
    jal x0, HIFI_CH4_IRQ_Handler     // 11
    jal x0, HIFI_CH5_IRQ_Handler     // 12
    jal x0, UART_IRQ_Handler         // 13
    jal x0, SPI_MASTER_IRQ_Handler   // 14
    jal x0, SPI_SLAVE_IRQ_Handler    // 15
    jal x0, I2C_IRQ_Handler          // 16 
    jal x0, WDT_IRQ_Handler          // 17
    jal x0, GPIO0_IRQ_Handler        // 18
    jal x0, GPIO1_IRQ_Handler        // 19 
    jal x0, GPIO2_IRQ_Handler        // 20 
    jal x0, GPIO3_IRQ_Handler        // 21
    jal x0, I2S_IRQ_Handler          // 22 
    jal x0, TIMER0_IRQ_Handler       // 23
    jal x0, TIMER1_IRQ_Handler       // 24
    jal x0, CRYPTO_IRQ_Handler       // 25
    jal x0, PDM_IRQ_Handler          // 26
    jal x0, VAD_IRQ_Handler          // 27
    jal x0, SAR_ADC_IRQ_Handler      // 28
    jal x0, IRQ_Handler              // 29 
    jal x0, IRQ_Handler              // 30  
    jal x0, SysTick_IRQ_Handler      // 31

    // reset vector
    jal x0, Reset_Handler

    // Illegal instrution exception
    jal x0, IllegalInstruction_Handler

    // ecall handler
    jal x0, Ecall_Handler

    // LSU error
    jal x0, LSU_Handler

    /*.section .startup*/

/* Reset Handler */
Reset_Handler:

    # Disable global interrupt. */
    csrci mstatus, 8

#ifdef __BOOT_LOADER__
    # copy bootloader from flash to sram
    li      a5,0x10000000    // source base address
    addi    a5,a5,64         // source address
    addi    a5,a5,0          // source address
    li      a4,0x20000000    // dest address
    li      a3,0x20000       // length
.LOOP1:
    lw      a0,0(a5)
    sw      a0,0(a4)
    addi    a5,a5,4
    addi    a4,a4,4
    addi    a3,a3,-4
    bnez    a3, .LOOP1
.LOOP2:
    nop

.LOOP3:
    j       0x200000d8      // ?????
    #j        .LOOP3
#endif

Entry:        
    # initialize stack pointer
    la sp, __stack_top__

    # initialize global pointer
    la gp, __global_pointer$


#ifndef __NO_SYSTEM_INIT
    jal SystemInit
#endif
   
    call __libc_init_array
    
    # Enable global interrupt. */
    /* csrsi mstatus, 8 */
    jal main
    
    //ebreak

bach2c_loop:
	jal	bach2c_loop

    .size Reset_Handler, . - Reset_Handler

    .global _init
    .global _fini
_init:
_fini:
    ret

  // saves all caller-saved registers (except return address)
store_regs:
    sw  x3, 0x00(x2)  // gp
    sw  x4, 0x04(x2)  // tp
    sw  x5, 0x08(x2)  // t0
    sw  x6, 0x0c(x2)  // t1
    sw  x7, 0x10(x2)  // t2
    sw x10, 0x14(x2)  // a0
    sw x11, 0x18(x2)  // a1
    sw x12, 0x1c(x2)  // a2
    sw x13, 0x20(x2)  // a3
    sw x14, 0x24(x2)  // a4
    sw x15, 0x28(x2)  // a5
    sw x16, 0x2c(x2)  // a6
    sw x17, 0x30(x2)  // a7

    csrr a0, 0x7B0
    csrr a1, 0x7B1
    csrr a2, 0x7B2
    sw a0, 0x34(x2)  // lpstart[0]
    sw a1, 0x38(x2)  // lpend[0]
    sw a2, 0x3c(x2)  // lpcount[0]
    csrr a0, 0x7B4
    csrr a1, 0x7B5
    csrr a2, 0x7B6
    sw a0, 0x40(x2)  // lpstart[1]
    sw a1, 0x44(x2)  // lpend[1]
    sw a2, 0x48(x2)  // lpcount[1]

    csrr a0, 0x341
    sw a0, 0x4c(x2)  // mepc
    csrr a1, 0x300
    sw a1, 0x50(x2)  // mstatus
    jalr x0, x1

    // load back registers from stack
end_except:
    lw a1, 0x50(x2)  // mstatus
    csrrw x0, 0x300, a1
    lw a0, 0x4c(x2)  // mepc
    csrrw x0, 0x341, a0

    lw a0, 0x40(x2)  // lpstart[1]
    lw a1, 0x44(x2)  // lpend[1]
    lw a2, 0x48(x2)  // lpcount[1]
    csrrw x0, 0x7B4, a0
    csrrw x0, 0x7B5, a1
    csrrw x0, 0x7B6, a2
    lw a0, 0x34(x2)  // lpstart[0]
    lw a1, 0x38(x2)  // lpend[0]
    lw a2, 0x3c(x2)  // lpcount[0]
    csrrw x0, 0x7B0, a0
    csrrw x0, 0x7B1, a1
    csrrw x0, 0x7B2, a2

    lw  x3, 0x00(x2)  // gp
    lw  x4, 0x04(x2)  // tp
    lw  x5, 0x08(x2)  // t0
    lw  x6, 0x0c(x2)  // t1
    lw  x7, 0x10(x2)  // t2
    lw x10, 0x14(x2)  // a0
    lw x11, 0x18(x2)  // a1
    lw x12, 0x1c(x2)  // a2
    lw x13, 0x20(x2)  // a3
    lw x14, 0x24(x2)  // a4
    lw x15, 0x28(x2)  // a5
    lw x16, 0x2c(x2)  // a6
    lw x17, 0x30(x2)  // a7

    lw  x1, 0x54(x2)
    addi x2, x2, EXCEPTION_STACK_SIZE
    mret

    .weak IRQ_Handler
    .type IRQ_Handler, %function
IRQ_Handler:
    addi x2, x2, -EXCEPTION_STACK_SIZE
    sw x1, 0x54(x2)
    jal x1, store_regs
    la x1, end_except
    csrr a0, mcause
    jal x0, SystemIrqHandler
    .size IRQ_Handler, . - IRQ_Handler

    .macro define_exception_entry entry_name handler_name
    .weak \entry_name
\entry_name:
    addi x2, x2, -EXCEPTION_STACK_SIZE
    sw x1, 0x54(x2)
    jal x1, store_regs
    la x1, end_except
    jal x0, \handler_name
    .endm

define_exception_entry IllegalInstruction_Handler IllegalInstruction_HandlerFunc
define_exception_entry Ecall_Handler Ecall_HandlerFunc
define_exception_entry LSU_Handler LSU_HandlerFunc

    .weak IllegalInstruction_HandlerFunc
    .type IllegalInstruction_HandlerFunc, %function
IllegalInstruction_HandlerFunc:
    j .
    .size IllegalInstruction_HandlerFunc, . - IllegalInstruction_HandlerFunc

    .weak Ecall_HandlerFunc
    .type Ecall_HandlerFunc, %function
Ecall_HandlerFunc:
    j .
    .size Ecall_HandlerFunc, . - Ecall_HandlerFunc

    .weak LSU_HandlerFunc
    .type LSU_HandlerFunc, %function
LSU_HandlerFunc:
    j .
    .size LSU_HandlerFunc, . - LSU_HandlerFunc
