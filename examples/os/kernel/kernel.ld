ENTRY(0)

SECTIONS {
    . = 0x100000;
    .text : {
        KEEP(*(.start));
        *(.text);
    }
    .rodata : {
        *(.rodata);
    }
    . = ALIGN(4K);
    .data : {
        *(.data);
    }
    .bss : {
        kernel_bssStart = .;
        *(.bss);
        kernel_bssEnd = .;
    }
    .uninit : {
        *(.uninit);
    }
    kernel_stackBottom = .;
    . = . + 1M;
    . = ALIGN(16);
    kernel_stackTop = .;
    /DISCARD/ : {
        *(.comment);
    }
}