Allocating a char array and accessing it properly within the Instruction RAM (IRAM) of ESP32.

This example follows the requirement of:

`The CPU can access data via the data bus in a byte-, half-word-, or word-aligned manner. The CPU can also access data via the instruction bus, but only in a word-aligned manner; non-word-aligned access will cause a CPU exception`

This example solves the long issue i had to be able to allocate variables and use them (i.e char array)

## Summary

The ESP32 IRAM is only accessible in word-aligned (4-byte) chunks, leading to exceptions when accessing data byte-by-byte.

## Solution:

- Use a union to access data in 4-byte chunks.
- Ensure variables like hex_digits are word-aligned.

## Key Points

### Word-Aligned Access:
- Use a union to safely handle 4-byte aligned access.
- This prevents exceptions caused by non-word-aligned memory accesses in IRAM.

### Loading Binary:
- Load the raw binary into the ESP32’s IRAM using heap_caps_malloc.
- Execute the binary via a function pointer.

### Accessing Data:
- Calculate the absolute address of variables in the binary by adding the base address.
- Read data in word-aligned chunks to avoid exceptions.
  
