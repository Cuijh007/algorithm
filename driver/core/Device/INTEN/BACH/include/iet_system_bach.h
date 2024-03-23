/*****************************************************************************
*  System Head File for BACH2.
*
*  Licensed under the GNU Lesser General Public License, version 2.1,
*  not use this file except in compliance with the License.
*
*  You should have received a copy of the GNU General Public License
*  along with this program. If not, see <http://www.gnu.org/licenses/>.
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
*  @file        iet_system_bach.h
*  @brief       iet bach system File
*  @author      Zenghua.Gao
*  @email       zhgao@intenginetech.com
*  @version     1.0
*  @date        2020-8-28
*  @license     GNU Lesser General Public License(LGPL), version 2.1
*
*---------------------------------------------------------------------------
*  Remark         : Description
*----------------------------------------------------------------------------
*  Change History :
*  <Date>     | <Version> | <Author>       | <Description>
*----------------------------------------------------------------------------
*  2020/08/28 | 1.0       | Zenghua.Gao    | Create file
*----------------------------------------------------------------------------
*
*****************************************************************************/
#ifndef __SYSTEM_BACH_H__
#define __SYSTEM_BACH_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BACH_IRQN_NUM       (0x1fUL)            // bach int number
#define BACH_EXT_INT_ADDR   (0x80000000)        // bach ext int addr

typedef enum IRQn
{
    /* ----------------------  BACH Specific Interrupt Numbers  --------------------- */

    DMAC_IRQn         = 0,
    MDF_AEC_IRQn      = 1,
    DMAC_CH0_IRQn     = 2,
    DMAC_CH1_IRQn     = 3,
    DMAC_CH2_IRQn     = 4,
    ADC_I2S_IRQn      = 5,
    VPU_IRQn          = 6,
    HIFI_0_IRQn       = 7,
    HIFI_1_IRQn       = 8,
    HIFI_2_IRQn       = 9,
    HIFI_3_IRQn       = 10,
    HIFI_4_IRQn       = 11,
    HIFI_5_IRQn       = 12,
    UART_IRQn         = 13,     /* UART0 Interrupt */
    SPI_MASTER_IRQn   = 14,
    SPI_SLAVE_IRQn    = 15,
    I2C_IRQn          = 16,
    WDT_IRQn          = 17,
    GPIO0_IRQn        = 18,     /* GPIOA0 Interrupt */
    GPIO1_IRQn        = 19,     /* GPIOA0 Interrupt */
    GPIO2_IRQn        = 20,     /* GPIOA0 Interrupt */
    GPIO3_IRQn        = 21,     /* GPIOA0 Interrupt */
    I2S_IRQn          = 22,
    TIMER0_IRQn       = 23,
    TIMER1_IRQn       = 24,
    CRYPTO_IRQn       = 25,
    PDM_IRQn          = 26,
    VAD_IRQn          = 27,
    SAR_ADC_IRQn      = 28,
    SYSTICK_IRQN      = 31,
} IRQn_Type;

#define DEFAULT_SYSTEM_CLOCK           50000000u           /* Default System clock value */

#define __STATIC_INLINE                static inline

//---------------------------------------------------------------------------
// Common macro definition
//---------------------------------------------------------------------------
#define     __IM     volatile const      //*! Defines 'read only' structure member permissions */
#define     __OM     volatile            //*! Defines 'write only' structure member permissions */
#define     __IOM    volatile            //*! Defines 'read/write' structure member permissions */

/* check device defines and use defaults */
#ifndef __NVIC_PRIO_BITS
#define __NVIC_PRIO_BITS          3U
//#warning "__NVIC_PRIO_BITS not defined in device header file; using default!"
#endif
#define IET_SYSTICK_TIMER_EN_CFG        (0x11)  // timer enable and generate period interrupt continue
#define IET_SYSTICK_TIMER_DIS           (0x0)  // timer disable

/*******************************************************************************
 *          BACH System Register Abstraction
  System Register contain:
  - System SCB Register
  - System SCC Register
  - System SysTick Register
  - System NVIC Register
 ******************************************************************************/
/**
  \brief  Structure type to access the System Control Block (SCB).
 */
typedef struct
{
    __IOM uint32_t SCER;                   /*!< Offset: 0x000 (R/W)  System Clock Enable Register */
    __IOM uint32_t SCRR;                   /*!< Offset: 0x004 (R/W)  System Core Reset Register */
    __IOM uint32_t VTOR;                   /*!< Offset: 0x008 (R/W)  Vector Table Offset Register */
    __IOM uint32_t SCFER;                  /*!< Offset: 0x00C (R/W)  System Core Fetch Enable Register */
    uint32_t RESERVED0;              /*!< Offset: 0x010 (R/W)  System reserved0 Register  */
    __IOM uint32_t SMARK0;                 /*!< Offset: 0x014 (R/W)  System Mark0 Register  */
    __IOM uint32_t SMARK1;                 /*!< Offset: 0x018 (R/W)  System Mark1 Register  */
    __IOM uint32_t SMARK2;                 /*!< Offset: 0x01C (R/W)  System Mark2 Register  */
} SCB_Type;


/**
  \brief  Structure type to access the System Cache Control (SCC).
 */
typedef struct
{
    __IOM uint32_t SPER;                   /*!< Offset: 0x100 (R/W)  System PCC Enable Register */
    __IOM uint32_t SPIR;                   /*!< Offset: 0x104 (R/W)  System PCC Invalidate Register */
} SCC_Type;


/**
  \brief  Structure type to access the System Timer (SysTick).
 */
typedef struct
{
    __IOM uint32_t CTRL;                   /*!< Offset: 0x000 (R/W)  SysTick Control Register */
    __IOM uint32_t LOAD;                   /*!< Offset: 0x004 (R/W)  SysTick Reload Value Register */
    __IOM uint32_t DIV;                    /*!< Offset: 0x00C (R/W)  SysTick Divide Register */
    __IOM uint32_t VAL;                    /*!< Offset: 0x008 (R/W)  SysTick Current Value Register */
} SysTick_Type;

#define SysTick_CTRL_ENABLE_Pos             0U                                            /*!< SysTick CTRL: ENABLE Position */
#define SysTick_CTRL_ENABLE_Msk            (1UL /*<< SysTick_CTRL_ENABLE_Pos*/)           /*!< SysTick CTRL: ENABLE Mask */

/* SysTick Reload Register Definitions */
#define SysTick_LOAD_RELOAD_Pos             0U                                            /*!< SysTick LOAD: RELOAD Position */
#define SysTick_LOAD_RELOAD_Msk            (0xFFFFFFUL /*<< SysTick_LOAD_RELOAD_Pos*/)    /*!< SysTick LOAD: RELOAD Mask */

/* SysTick Current Register Definitions */
#define SysTick_VAL_CURRENT_Pos             0U                                            /*!< SysTick VAL: CURRENT Position */
#define SysTick_VAL_CURRENT_Msk            (0xFFFFFFUL /*<< SysTick_VAL_CURRENT_Pos*/)    /*!< SysTick VAL: CURREN */

/**
  \brief  Structure type to access the Nested Vectored Interrupt Controller (NVIC).
 */
typedef struct
{
    __IOM uint32_t IMR;                   /*!< Offset: 0x1000 (R/W)  Interrupt Mask Register */
    __IOM uint32_t ITR;                   /*!< Offset: 0x1004 (R/W)  Interrupt Type Register */
    __IOM uint32_t ISR;                   /*!< Offset: 0x1008 (R/W)  Interrupt Status Register */
    __IOM uint32_t ICR;                   /*!< Offset: 0x100C (R/W)  Interrupt Clear Register */
    __IOM uint32_t RESERVED0[28];         /*!< reserved */
    __IOM uint32_t IP[32U];               /*!< Offset: 0x1080 (R/W)  Interrupt Priority Register */
}  NVIC_Type;

/**
  \brief   Mask and shift a bit field value for use in a register bit range.
  \param[in] field  Name of the register bit field.
  \param[in] value  Value of the bit field. This parameter is interpreted as an uint32_t type.
  \return           Masked and shifted value.
*/
#define _VAL2FLD(field, value)    (((uint32_t)(value) << field ## _Pos) & field ## _Msk)

/**
  \brief     Mask and shift a register value to extract a bit filed value.
  \param[in] field  Name of the register bit field.
  \param[in] value  Value of register. This parameter is interpreted as an uint32_t type.
  \return           Masked and shifted bit field value.
*/
#define _FLD2VAL(field, value)    (((uint32_t)(value) & field ## _Msk) >> field ## _Pos)

/* Memory mapping of System Hardware */
#define SCS_BASE            (0xE0000000UL)                            /*!< System Control Space Base Address */
#define SCB_BASE            (SCS_BASE +  0x0000UL)                    /*!< System Control Block Base Address */
#define SCC_BASE            (SCS_BASE +  0x0100UL)                    /*!< System Cache Control Base Address */
#define SysTick_BASE        (SCS_BASE +  0x0300UL)                    /*!< SysTick Base Address */
#define NVIC_BASE           (SCS_BASE +  0x1000UL)                    /*!< NVIC Base Address */

#define SCB                 ((SCB_Type*) SCB_BASE )                   /*!< SCB configuration struct */
#define SCC                 ((SCC_Type*) SCC_BASE )                   /*!< SCC configuration struct */
#define SysTick             ((SysTick_Type*) SysTick_BASE)            /*!< SysTick configuration struct */
#define NVIC                ((NVIC_Type*) NVIC_BASE )                 /*!< NVIC configuration struct */

/*******************************************************************************
 *                System Abstraction Layer
  System Function Interface contains:
  - System NVIC Functions
  - System SysTick Functions
  - System Register Access Functions
 ******************************************************************************/
#define ICACHE_Enable              __ICACHE_Enable
#define ICACHE_Disable             __ICACHE_Disable
#define ICACHE_Invalidate          __ICACHE_Invalidate


/* ##########################   NVIC functions  #################################### */
/**
  \ingroup  FunctionInterface
  \defgroup NVICFunctions NVIC Functions
  \brief    Functions that manage interrupts and exceptions via the NVIC.
  @{
 */
#define NVIC_EnableIRQ              __NVIC_EnableIRQ                  //
#define NVIC_GetEnableIRQ           __NVIC_GetEnableIRQ               //
#define NVIC_DisableIRQ             __NVIC_DisableIRQ                 //
#define NVIC_GetPendingIRQ          __NVIC_GetPendingIRQ
#define NVIC_SetPendingIRQ          __NVIC_SetPendingIRQ
#define NVIC_ClearPendingIRQ        __NVIC_ClearPendingIRQ
#define NVIC_GetActive              __NVIC_GetActive
#define NVIC_SetType                __NVIC_SetType                    //
#define NVIC_GetType                __NVIC_GetType                    //
#define NVIC_SetPriority            __NVIC_SetPriority                //
#define NVIC_GetPriority            __NVIC_GetPriority                //
#define NVIC_SystemReset            __NVIC_SystemReset                //

#define NVIC_SetVectorTable         __NVIC_SetVectorTable             // 

#define NVIC_SetVector              __NVIC_SetVector                  // 
#define NVIC_GetVector              __NVIC_GetVector                  //  

#define NVIC_USER_IRQ_OFFSET        0

#define GET_REG(r)                    (*((volatile unsigned int*) (r)))
#define SET_REG(r,v)                  (*((volatile unsigned int*) (r)) = ((unsigned int)   (v)))
#define SET_BIT(X,Y)                  ((*(volatile unsigned int*)X) |=(0x00000001L<<(Y)))
#define CLR_BIT(X,Y)                  ((*(volatile unsigned int*)X) &=(~(0x00000001L<<(Y))))
#define W1C_BIT(X,Y)                  ((*(volatile unsigned int*)X) =(0x00000001L<<(Y)))

/**
  \brief   ICACHE Enable
  \details Enables icache.
  \param
  \note
 */
__STATIC_INLINE void __ICACHE_Enable()
{
    SCC->SPER = (uint32_t) 0x1;
}

/**
  \brief   ICACHE Disable
  \details Enables icache.
  \param
  \note
 */
__STATIC_INLINE void __ICACHE_Disable()
{
    SCC->SPER = (uint32_t) 0x0;
}

/**
  \brief   ICACHE Enable
  \details Enables icache.
  \param
  \note
 */
__STATIC_INLINE void __ICACHE_Invalidate()
{
    SCC->SPIR = (uint32_t) 0x1;
}


/**
  \brief   Enable Interrupt
  \details Enables a device specific interrupt in the NVIC interrupt controller.
  \param   [in]      IRQn  Device specific interrupt number.
  \note    IRQn must not be negative.
 */
__STATIC_INLINE void __NVIC_EnableIRQ(IRQn_Type IRQn)
{
    if ((int32_t)(IRQn) >= 0)
    {
        NVIC->IMR &= (uint32_t)(~(1UL << (((uint32_t)IRQn) & 0x1FUL)));
    }
}


/**
  \brief   Get Interrupt Enable status
  \details Returns a device specific interrupt enable status from the NVIC interrupt controller.
  \param   [in]      IRQn  Device specific interrupt number.
  \return             0  Interrupt is not enabled.
  \return             1  Interrupt is enabled.
  \note    IRQn must not be negative.
 */
__STATIC_INLINE uint32_t __NVIC_GetEnableIRQ(IRQn_Type IRQn)
{
    if ((int32_t)(IRQn) >= 0)
    {
        return (uint32_t)(((NVIC->IMR & (((1UL << ((uint32_t)IRQn))) & 0x1FUL)) == 0UL) ? 1UL : 0UL);
    }
    else
    {
        return (0U);
    }
}

/**
  \brief   Disable Interrupt
  \details Disables a device specific interrupt in the NVIC interrupt controller.
  \param   [in]      IRQn  Device specific interrupt number.
  \note    IRQn must not be negative.
 */
__STATIC_INLINE void __NVIC_DisableIRQ(IRQn_Type IRQn)
{
    if ((int32_t)(IRQn) >= 0)
    {
        NVIC->IMR |= (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    }
}

/**
  \brief   Get Pending Interrupt
  \details Reads the NVIC pending register and returns the pending bit for the specified device specific interrupt.
  \param   [in]      IRQn  Device specific interrupt number.
  \return             0  Interrupt status is not pending.
  \return             1  Interrupt status is pending.
  \note    IRQn must not be negative.
 */
__STATIC_INLINE uint32_t __NVIC_GetPendingIRQ(IRQn_Type IRQn)
{
    if ((int32_t)(IRQn) >= 0)
    {
        return (((uint32_t)(NVIC->ISR & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL);
    }
    else
    {
        return (0U);
    }
}


/**
  \brief   Set Pending Interrupt
  \details Sets the pending bit of a device specific interrupt in the NVIC pending register.
  \param   [in]      IRQn  Device specific interrupt number.
  \note    IRQn must not be negative.
 */
__STATIC_INLINE void __NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
    if ((int32_t)(IRQn) >= 0)
    {

    }
}


/**
  \brief   Clear Pending Interrupt
  \details Clears the pending bit of a device specific interrupt in the NVIC pending register.
  \param [in]      IRQn  Device specific interrupt number.
  \note    IRQn must not be negative.
 */
__STATIC_INLINE void __NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
    if ((int32_t)(IRQn) >= 0)
    {
        NVIC->IMR |= (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    }
}


/**
  \brief   Get Active Interrupt
  \details Reads the active register in the NVIC and returns the active bit for the device specific interrupt.
  \param   [in]      IRQn  Device specific interrupt number.
  \return             0  Interrupt status is not active.
  \return             1  Interrupt status is active.
  \note    IRQn must not be negative.
 */
__STATIC_INLINE uint32_t __NVIC_GetActive(IRQn_Type IRQn)
{
    if ((int32_t)(IRQn) >= 0)
    {

    }
    else
    {
        return (0U);
    }
}

/**
  \brief   Set Interrupt Type
  \details Sets the type of a device specific interrupt or a processor exception.
           The interrupt number is positive which specifies a device specific interrupt
  \param   [in]  IRQn    Interrupt number.
  \param   [in]  type    Type to set. 0 : level; 1 : pulse
  \note
 */
__STATIC_INLINE void __NVIC_SetType(IRQn_Type IRQn, uint32_t type)
{
    if ((int32_t)(IRQn) >= 0)
    {
        if (0x1 == type)
        {
            NVIC->ITR |= (uint32_t)(0x1 << IRQn);
        }
        else if (0x0 == type)
        {
            NVIC->ITR &= (uint32_t)(~(0x1 << IRQn));
        }
        else
        {

        }
    }
    else
    {

    }
}

/**
  \brief   Get Interrupt Type
  \details Reads the type of a device specific interrupt
           The interrupt number is positive which specify a device specific interrupt,
  \param [in]   IRQn  Interrupt number.
  \return             Interrupt Type
 */
__STATIC_INLINE uint32_t __NVIC_GetType(IRQn_Type IRQn)
{

    if ((int32_t)(IRQn) >= 0)
    {
        if ((uint32_t)NVIC->ITR)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
    }
}

/**
  \brief   Set Interrupt Priority
  \details Sets the priority of a device specific interrupt or a processor exception.
           The interrupt number can be positive to specify a device specific interrupt,
           or negative to specify a processor exception.
  \param   [in]    IRQn    Interrupt number.
  \param   [in]  priority  Priority to set.
  \note    The priority cannot be set for every processor exception.
 */
__STATIC_INLINE void __NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
    if ((int32_t)(IRQn) >= 0)
    {
        NVIC->IP[((uint32_t)IRQn)] = (uint8_t)(priority & 0x7UL);
    }
    else
    {

    }
}

/**
  \brief   Get Interrupt Priority
  \details Reads the priority of a device specific interrupt or a processor exception.
           The interrupt number can be positive to specify a device specific interrupt,
           or negative to specify a processor exception.
  \param [in]   IRQn  Interrupt number.
  \return             Interrupt Priority.
                      Value is aligned automatically to the implemented priority bits of the microcontroller.
 */
__STATIC_INLINE uint32_t __NVIC_GetPriority(IRQn_Type IRQn)
{

    if ((int32_t)(IRQn) >= 0)
    {
        return ((uint32_t)NVIC->IP[((uint32_t)IRQn)]);
    }
    else
    {
    }
}

/**
  \brief   Update Interrupt Vector Address
  \details
  \param [in]   interrupt vector address
 */
__STATIC_INLINE void __NVIC_SetVectorTable(uint32_t addr)
{
    SET_REG(0x51b00010, addr);
}


/**
  \brief   Set Interrupt Vector
  \details Sets an interrupt vector in SRAM based interrupt vector table.
           The interrupt number can be positive to specify a device specific interrupt,
           or negative to specify a processor exception.
           VTOR must been relocated to SRAM before.
  \param [in]   IRQn      Interrupt number
  \param [in]   vector    Address of interrupt handler function
 */
__STATIC_INLINE void __NVIC_SetVector(IRQn_Type IRQn, uint32_t vector)
{
    uint32_t *vectors = (uint32_t *)SCB->VTOR;
    vectors[(int32_t)IRQn + NVIC_USER_IRQ_OFFSET] = vector;
}


/**
  \brief   Get Interrupt Vector
  \details Reads an interrupt vector from interrupt vector table.
           The interrupt number can be positive to specify a device specific interrupt,
           or negative to specify a processor exception.
  \param [in]   IRQn      Interrupt number.
  \return                 Address of interrupt handler function
 */
__STATIC_INLINE uint32_t __NVIC_GetVector(IRQn_Type IRQn)
{
    uint32_t *vectors = (uint32_t *)SCB->VTOR;
    return vectors[(int32_t)IRQn + NVIC_USER_IRQ_OFFSET];
}


/**
  \brief   System Reset
  \details Initiates a system reset request to reset the MCU.
 */
__STATIC_INLINE void __NVIC_SystemReset(void)
{
    /* Ensure all outstanding memory accesses included
     buffered write are completed before reset */
    SCB->SCRR  = (uint32_t)0x1UL;
    /* Ensure completion of memory access */

    for (;;)                                                            /* wait until reset */
    {
        //__NOP();
    }
}

/* ##########################  FPU functions  #################################### */
/**
  \brief   get FPU type
  \details returns the FPU type
  \returns
   - \b  0: No FPU
   - \b  1: Single precision FPU
   - \b  2: Double + Single precision FPU
 */
__STATIC_INLINE uint32_t SCB_GetFPUType(void)
{
    return 0U;           /* No FPU */
}


/* ##################################    SysTick function  ############################################ */
/**
  \brief   System Tick Configuration
  \details Initializes the System Timer and its interrupt, and starts the System Tick Timer.
           Counter is in free running mode to generate periodic interrupts.
  \param [in]  ticks  Number of ticks between two interrupts.
  \return          0  Function succeeded.
  \return          1  Function failed.
  \note
 */
__STATIC_INLINE uint32_t SysTick_Config(uint32_t ticks)
{
    if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk)
    {
        return (1UL);                                                   /* Reload value impossible */
    }

    SysTick->LOAD  = (uint32_t)(ticks - 1UL);                         /* set reload register */
    NVIC_SetPriority(SYSTICK_IRQN, (1UL << __NVIC_PRIO_BITS) - 1UL);  /* set Priority for Systick Interrupt */
    SysTick->DIV   = 0UL;
    SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
    SysTick->CTRL  = SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */
    return (0UL);                                                     /* Function successful */
}
/**
  \fn           void riscv_timer_set_ctrl(uint32_t control);
  \brief        set timer config
  \param[in]    control             timer config
  \return       NULL
*/
extern void riscv_timer_set_ctrl(uint32_t control);
/**
  \fn           int32_t iet_dsmdac_start(void)
  \brief        set timer load config
  \param[in]    value               timer load config
  \return       NULL
*/

extern void riscv_timer_set_load(uint32_t value);
#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_BACH_H__ */

/* ------------------------------ End Of File ------------------------------ */
