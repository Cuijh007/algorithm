/*
 * Copyright 2019 ietengine
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __CORE_RISCV_H__
#define __CORE_RISCV_H__

#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

#define RISCV32

#if defined ( __GNUC__ )
  #define __ASM            __asm                                      /*!< asm keyword for GNU Compiler */
  #define __INLINE         inline                                     /*!< inline keyword for GNU Compiler */
  #define __STATIC_INLINE  static inline

#else
  #error Unknown compiler
#endif

#if defined ( __GNUC__ )

#define __BKPT(x)              __ASM("ebreak")

__attribute__((always_inline)) __STATIC_INLINE void __NOP(void)
{
  __ASM volatile ("nop");
}

__attribute__((always_inline)) __STATIC_INLINE void __DSB(void)
{
  __ASM volatile ("nop");
}

__attribute__((always_inline)) __STATIC_INLINE void __ISB(void)
{
  __ASM volatile ("nop");
}

__attribute__((always_inline)) __STATIC_INLINE void __WFI(void)
{

}

__attribute__((always_inline)) __STATIC_INLINE void __WFE(void)
{
}

__attribute__( ( always_inline ) ) __STATIC_INLINE void __enable_irq(void)
{
  __ASM volatile ("csrsi mstatus, 8");
}

__attribute__( ( always_inline ) ) __STATIC_INLINE void __disable_irq(void)
{
  __ASM volatile ("csrci mstatus, 8");
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t __REV(uint32_t value)
{
  return __builtin_bswap32(value);
}

__attribute__((always_inline)) __STATIC_INLINE uint32_t __REV16(uint32_t value)
{
  return __builtin_bswap16(value);
}

#define read_csr(reg) ({ unsigned long __tmp; \
  asm volatile ("csrr %0, " #reg : "=r"(__tmp)); \
  __tmp; })

#define write_csr(reg, val) ({ \
  asm volatile ("csrw " #reg ", %0" :: "rK"(val)); })

#define swap_csr(reg, val) ({ unsigned long __tmp; \
  asm volatile ("csrrw %0, " #reg ", %1" : "=r"(__tmp) : "rK"(val)); \
  __tmp; })

#define set_csr(reg, bit) ({ unsigned long __tmp; \
  asm volatile ("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "rK"(bit)); \
  __tmp; })

#define clear_csr(reg, bit) ({ unsigned long __tmp; \
  asm volatile ("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "rK"(bit)); \
  __tmp; })

#define rdtime()               read_csr(time)
#define rdcycle()              read_csr(cycle)
#define rdinstret()            read_csr(instret)

#endif


#ifdef __cplusplus
  #define   __I     volatile             /*!< Defines 'read only' permissions */
#else
  #define   __I     volatile const       /*!< Defines 'read only' permissions */
#endif

#define     __O     volatile             /*!< Defines 'write only' permissions */
#define     __IO    volatile             /*!< Defines 'read / write' permissions */

/* following defines should be used for structure members */
#define     __IM    volatile const       /*! Defines 'read only' structure member permissions */
#define     __OM    volatile             /*! Defines 'write only' structure member permissions */
#define     __IOM   volatile             /*! Defines 'read / write' structure member permissions */

#ifdef __cplusplus
}
#endif

#endif /* __CORE_RISCV_H__ */
