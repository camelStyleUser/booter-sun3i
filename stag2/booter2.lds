SECTIONS
{
	ENTRY(enter)
        . = 0x80000000;
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
