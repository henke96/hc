ENTRY(0)

SECTIONS {
    . = 0;
    .text : {
        KEEP(*(.start));
        *(.text*);
    }
    .rodata : {
        *(.rodata*);
    }
    . = ALIGN(4K);
    .data : {
        *(.data*);
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
    . = . + 16K;
    . = ALIGN(16);
    kernel_stackTop = .;
    ASSERT(. <= 0x200000, "Using too much memory")
    /DISCARD/ : {
        *(.comment);
    }
}