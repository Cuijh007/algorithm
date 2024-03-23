#include "BACH.h"
#include "system_BACH.h"
#include "iet_common.h"

/* ----------------------------------------------------------------------------
   -- Core clock
   ---------------------------------------------------------------------------- */
uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;

static uint32_t irqNesting = 0;

extern uint32_t __VECTOR_TABLE[];
extern uint32_t __etext;
extern uint32_t __data_start__;
extern uint32_t __data_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;

extern void iet_systick_irq_handler();


typedef void (*irq_handler_t)(void);

static void DefaultIRQHandler(void)
{
    for (;;)
    {

    }
}

#define DEFINE_IRQ_HANDLER(irq_handler, driver_irq_handler) \
    void __attribute__((weak)) __attribute__((interrupt)) irq_handler(void) { driver_irq_handler();}

#define DEFINE_DEFAULT_IRQ_HANDLER(irq_handler) void __attribute__((interrupt)) irq_handler() __attribute__((weak, alias("DefaultIRQHandler")))

DEFINE_DEFAULT_IRQ_HANDLER(DEFAULT_IRQ_Handler);
DEFINE_DEFAULT_IRQ_HANDLER(DMAC_IRQ_Handler);         // 0
DEFINE_DEFAULT_IRQ_HANDLER(DMAC_COMMON_IRQ_Handler);  // 1       
DEFINE_DEFAULT_IRQ_HANDLER(DMAC_CH0_IRQ_Handler);     // 2    
DEFINE_DEFAULT_IRQ_HANDLER(DMAC_CH1_IRQ_Handler);     // 3           
DEFINE_DEFAULT_IRQ_HANDLER(DMAC_CH2_IRQ_Handler);     // 4
DEFINE_DEFAULT_IRQ_HANDLER(DMAC_CH3_IRQ_Handler);     // 5
DEFINE_DEFAULT_IRQ_HANDLER(VPU_IRQ_Handler);          // 6
//DEFINE_DEFAULT_IRQ_HANDLER(HIFI_CH0_IRQ_Handler);        // 7 
//DEFINE_DEFAULT_IRQ_HANDLER(HIFI_CH1_IRQ_Handler);        // 8
//DEFINE_DEFAULT_IRQ_HANDLER(HIFI_CH2_IRQ_Handler);        // 9
//DEFINE_DEFAULT_IRQ_HANDLER(HIFI_CH3_IRQ_Handler);        // 10 
//DEFINE_DEFAULT_IRQ_HANDLER(HIFI_CH4_IRQ_Handler);        // 11
//DEFINE_DEFAULT_IRQ_HANDLER(HIFI_CH5_IRQ_Handler);        // 12
DEFINE_DEFAULT_IRQ_HANDLER(UART_IRQ_Handler);         // 13
DEFINE_DEFAULT_IRQ_HANDLER(SPI_MASTER_IRQ_Handler);   // 14
DEFINE_DEFAULT_IRQ_HANDLER(SPI_SLAVE_IRQ_Handler);    // 15
DEFINE_DEFAULT_IRQ_HANDLER(I2C_IRQ_Handler);          // 16 
DEFINE_DEFAULT_IRQ_HANDLER(WDT_IRQ_Handler);          // 17
DEFINE_DEFAULT_IRQ_HANDLER(GPIO0_IRQ_Handler);        // 18
DEFINE_DEFAULT_IRQ_HANDLER(GPIO1_IRQ_Handler);        // 19 
DEFINE_DEFAULT_IRQ_HANDLER(GPIO2_IRQ_Handler);        // 20 
DEFINE_DEFAULT_IRQ_HANDLER(GPIO3_IRQ_Handler);        // 21
DEFINE_DEFAULT_IRQ_HANDLER(I2S_IRQ_Handler);          // 22 
//DEFINE_DEFAULT_IRQ_HANDLER(TIMER0_IRQ_Handler);       // 23
//DEFINE_DEFAULT_IRQ_HANDLER(TIMER1_IRQ_Handler);       // 24
DEFINE_DEFAULT_IRQ_HANDLER(CRYPTO_IRQ_Handler);       // 25
DEFINE_DEFAULT_IRQ_HANDLER(PDM_IRQ_Handler);          // 26
DEFINE_DEFAULT_IRQ_HANDLER(VAD_IRQ_Handler);          // 27
DEFINE_DEFAULT_IRQ_HANDLER(SAR_ADC_IRQ_Handler);      // 28
//DEFINE_DEFAULT_IRQ_HANDLER(SYSTICK_IRQ_Handler);      // 31


DEFINE_IRQ_HANDLER(HIFI_CH0_IRQ_Handler, iet_hifi_ch0_irq_handler);        // 7 
DEFINE_IRQ_HANDLER(HIFI_CH1_IRQ_Handler, iet_hifi_ch1_irq_handler);        // 8
DEFINE_IRQ_HANDLER(HIFI_CH2_IRQ_Handler, iet_hifi_ch2_irq_handler);        // 9
DEFINE_IRQ_HANDLER(HIFI_CH3_IRQ_Handler, iet_hifi_ch3_irq_handler);        // 10 
DEFINE_IRQ_HANDLER(HIFI_CH4_IRQ_Handler, iet_hifi_ch4_irq_handler);        // 11
DEFINE_IRQ_HANDLER(HIFI_CH5_IRQ_Handler, iet_hifi_ch5_irq_handler);        // 12
DEFINE_IRQ_HANDLER(TIMER0_IRQ_Handler,   iet_timer0_irq_handler);
DEFINE_IRQ_HANDLER(TIMER1_IRQ_Handler,   iet_timer1_irq_handler);
//DEFINE_IRQ_HANDLER(CRYPTO_IRQ_Handler, iet_crypto_irq_handler);       
DEFINE_IRQ_HANDLER(SysTick_IRQ_Handler, iet_systick_irq_handler);

// dont care,reserved for use later
__attribute__((section("user_vectors"))) const irq_handler_t isrTable[] =
{
    DefaultIRQHandler,  //0
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
    DefaultIRQHandler,
};

static void copy_section(uint32_t * p_lma, uint32_t * p_vma_start, uint32_t * p_vma_end)
{
    uint32_t * p_vma = p_vma_start;
    
    while(p_vma <= p_vma_end)
    {
        *p_vma = *p_lma;
        ++p_lma;
        ++p_vma;
    }
}

static void zero_section(uint32_t * start, uint32_t * end)
{
    uint32_t * p_zero = start;

    while(p_zero <= end)
    {
        *p_zero = 0;
        ++p_zero;
    }
}

// dont care,reserved for use later
__attribute__((weak)) void SystemIrqHandler(uint32_t mcause) 
{
    uint32_t intNum;
    
    if (mcause & 0x80000000) /* For external interrupt. */
    {
        intNum = mcause & 0x1FUL;

        irqNesting++;

        /* Clear pending flag in EVENT unit .*/
        
        /* Read back to make sure write finished. */
        
        //__enable_irq();      /* Support nesting interrupt */

        /* Now call the real irq handler for intNum */
        isrTable[intNum]();

        //__disable_irq();

        irqNesting--;
    }
}

/**
  * @brief  Setup the microcontroller system
  *         Initialize the FPU setting, vector table location and External memory 
  *         configuration.
  * @param  None
  * @retval None
  */


void SystemInit(void) 
{ 
#ifdef __CACHE__
  ICACHE_Invalidate();
  ICACHE_Enable();
#endif

  //20002218 g       .fini_array	00000000 __etext
  //20002218 g       .data	        00000000 __data_start__
  //20002374 g       .data	        00000000 __data_end__

  copy_section(&__etext, &__data_start__, &__data_end__);

  
  //20002374 g       .bss       	00000000 __bss_start__
  //20002418 g       .bss	        00000000 __bss_end__
  zero_section(&__bss_start__, &__bss_end__);

  SystemInitHook();

  

  SET_REG(0x51B00010,__VECTOR_TABLE); //
  //SET_REG(0x51b00010,0x20000000); //
  
  irqNesting = 0;
}

/* ----------------------------------------------------------------------------
   -- SystemInitHook()
   ---------------------------------------------------------------------------- */
__attribute__ ((weak)) void SystemInitHook (void) 
{
  /* Void implementation of the weak function. */
}

/**
   * @brief  Update SystemCoreClock variable according to Clock Register Values.
  *         The SystemCoreClock variable contains the core clock, it can
  *         be used by the user application to setup the SysTick timer or configure
  *         other parameters.
  *           
  * @note   Each time the core clock changes, this function must be called
  *         to update SystemCoreClock variable value. Otherwise, any configuration
  *         based on this variable will be incorrect.         
  *     
  * @note   
  *     
  * @param  None
  * @retval None
  */
void SystemCoreClockUpdate(void)
{

}

uint32_t SystemGetIRQNestingLevel(void)
{
    return irqNesting;
}


void SystemReset(void)
{

}
