SECTIONS
{
	ENTRY(_real_start)
        . = 0x0020;/*old mksunxiboot*/
        .text : { *(.text) }
	.data : { *(.data) }
	.bss : { *(.bss COMMON) }
	.rodata : { *(.rodata*) }
        /DISCARD/ : { *(.dynstr*) }
        /DISCARD/ : { *(.dynamic*) }
        /DISCARD/ : { *(.plt*) }
        /DISCARD/ : { *(.interp*) }
        /DISCARD/ : { *(.gnu*) }
        /DISCARD/ : { *(.note*) }
}
