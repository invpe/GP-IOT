SECTIONS {
    .task_metadata : {
        *(.task_metadata)  /* Place all symbols in the .task_metadata section */
    } > FLASH

    .literal : {
        *(.literal) /* Place all symbols in the .literal section */
    } > FLASH
    .text : {
        *(.text) /* Place all symbols in the .literal section */
    } > FLASH

    /* Other sections... */
}
