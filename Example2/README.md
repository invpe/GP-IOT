
## Summary

`The ESP32 IRAM is only accessible in word-aligned (4-byte) chunks, leading to exceptions when accessing data byte-by-byte.`

Allocating a char array and accessing it properly within the Instruction RAM (IRAM) of ESP32.

This example solves the long issue i struggled with to allocate variables and use them (i.e char array)

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

## Results

```
[12:28:54:721] BIN SIZE: 104
[12:28:54:721] Free heap size: 269524
[12:28:54:721] MALLOC OK
[12:28:54:728] File Size: 104
[12:28:54:728] Size of TaskMetadata: 20
[12:28:54:732] Program buffer address: 0x4008E580
[12:28:54:732] Metadata address: 0x4008E5D4
[12:28:54:735] Task Function Address in Metadata: 0x4
[12:28:54:737] Calculated Task Function Address: 0x4008E584
[12:28:54:741] Return value from task: 0123456789ABCDEF
[12:28:54:745] Offset of hex in taskFunction: 0x40
[12:28:54:745] Absolute Address of hex: 0x4008E5C0
```
  
