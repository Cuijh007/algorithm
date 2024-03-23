/* Linker script to configure memory regions. */
MEMORY
{
  rom (rx)     : ORIGIN = 0x00000000, LENGTH = 64K
  flash (rx)   : ORIGIN = 0x10000000, LENGTH = 1M
  sram(rwx)    : ORIGIN = 0x20000000, LENGTH = 256K
}

HEAP_SIZE  = DEFINED(__heap_size__)  ? __heap_size__  : 0x1000;
STACK_SIZE = DEFINED(__stack_size__) ? __stack_size__ : 0x0800;

/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a)

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   reset_handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __copy_table_start__
 *   __copy_table_end__
 *   __zero_table_start__
 *   __zero_table_end__
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __heap_base
 *   __heap_limit
 *   __stack_limit
 *   __stack_top
 *   __stack
 *   __vector_end
 *   __vector_size
 */
/*ENTRY(Reset_Handler)*/

SECTIONS
{
	.text ALIGN(4):
	{
        . = ALIGN(4);	
        __VECTOR_TABLE = .; 

	    KEEP(*(.vectors))
    
		*(.text*)

		KEEP(*(.init))
		KEEP(*(.fini))

		/* .ctors */
		*crtbegin.o(.ctors)
		*crtbegin?.o(.ctors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
		*(SORT(.ctors.*))
		*(.ctors)

		/* .dtors */
 		*crtbegin.o(.dtors)
 		*crtbegin?.o(.dtors)
 		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
 		*(SORT(.dtors.*))
 		*(.dtors)

		KEEP(*(.eh_frame*))
	} > rom
    
	.preinit_array :
    {
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    } > rom

    .init_array :
    {
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
    } > rom

    .fini_array :
    {
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);
    } > rom

    __etext = .;             /* define a global symbol at end of code */
    __DATA_ROM = .;          /* Symbol is used by startup for data initialization */

    .data :  AT(__etext)
  	{
		__data_start__ = .;
		*(.data*)

  	    . = ALIGN(4);
        PROVIDE( __global_pointer$ = . + (4K / 2) );
        *(.sdata .sdata.*)

		KEEP(*(.jcr*))
		
		*(.rodata*)
	
		/* All data end */
		__data_end__ = .;
	} > sram

    __DATA_END = __DATA_ROM + (__data_end__ - __data_start__);

    _edata = .;
    _fbss = .;
        
	.bss :
	{
		. = ALIGN(4);
		__bss_start__ = .;
        *(.bss)
        *(.bss*)
        *(.sbss)
        *(.sbss*)
        *(COMMON)

		. = ALIGN(4);
		__bss_end__ = .;
	} > sram


    /* End of uninitalized data segement */
    _end = .;
    PROVIDE(end = .);

    .heap :
    {
      . = ALIGN(8);
      __heap_start = .;
      . += HEAP_SIZE;
      __heap_end = .;
      _heap_end = __heap_end;
    } > sram


	/* .stack_dummy section doesn't contains any symbols. It is only
	 * used for linker to calculate size of stack sections, and assign
	 * values to stack symbols later */
	.stack_dummy (COPY):
	{
		KEEP(*(.stack*))
	} > sram

	/* set stack top to end of SRAM, and stack limit move down by
	 * size of stack_dummy section */
	__stack_top__ = ORIGIN(sram) + LENGTH(sram);
	__stack_limit__ = __stack_top__ - SIZEOF(.stack_dummy);
	PROVIDE(__stack__ = __stack_top__);

	/* Check if data + heap + stack exceeds RAM limit */
	ASSERT(__stack_limit__ >= _heap_end, "region sram overlapped with stack")
}
