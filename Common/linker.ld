/*
    Relative to the binary, so we don't refer to memory already taken by the RUNNER sketch
*/
MEMORY
{
    BINARY (RWX) : org = 0x00, len = 0xFFFF
}  
SECTIONS
{
    .literal : {* (.literal*) } > BINARY
    .text   : { *(.text*)   } > BINARY
    .rodata : { *(.rodata*) } > BINARY
    .bss    : { *(.bss*)    } > BINARY
    .data    : { *(.data*)    } > BINARY
    .task_metadata : { KEEP(*(.task_metadata)) } > BINARY
}
