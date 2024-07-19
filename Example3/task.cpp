#include <stdint.h>
#include <string.h>
#include <cstdio>
#include "../RUNNER/Common.h"

uintptr_t uiptrBaseAddress = 0x00;

// Macro for reading data with alignment consideration
#define READ_ALIGNED_DATA(base, offset, buffer, length) \
    do { \
        for (int i = 0; i < length; i += 4) { \
            union { char bytes[4]; uint32_t word; } data; \
            data.word = *((uint32_t*)((base) + (offset) + i)); \
            for (int j = 0; j < 4 && (i + j) < length; ++j) { \
                buffer[i + j] = data.bytes[j]; \
            } \
        } \
        buffer[length] = '\0'; \
    } while (0)
// Macro for writing data with alignment consideration
#define WRITE_ALIGNED_DATA(base, offset, buffer, length) \
    do { \
        for (int i = 0; i < length; i += 4) { \
            union { char bytes[4]; uint32_t word; } data; \
            for (int j = 0; j < 4 && (i + j) < length; ++j) { \
                data.bytes[j] = buffer[i + j]; \
            } \
            *((uint32_t*)((base) + (offset) + i)) = data.word; \
        } \
    } while (0)

const char hex_digits_LITERAL[] __attribute__((aligned(4))) = "LITERALL_VARIABLE"; // .literal
const char hex_digits_RODATA[] __attribute__((aligned(4))) = "RODATA_VARIABLE"; // .rodata
char hex_digits_DATA[] __attribute__((aligned(4))) = "DATA_VARIABLE"; // .data
char hex_digits_DATA2[] __attribute__((aligned(4))) = "DATA2_VARIABLE"; // .data

void test(char* output) {
    output[0] = 0x41; // 'A'
    output[1] = 0x42; // 'B'
    output[2] = '\0'; // Null-terminate
}
// Test accessing sections from a function
void test2(uintptr_t baseAddress, char* output) { 
    READ_ALIGNED_DATA(baseAddress + (uintptr_t)hex_digits_RODATA, 0, output, 16);
    READ_ALIGNED_DATA(baseAddress + (uintptr_t)hex_digits_DATA, 0, output, 16);    
    READ_ALIGNED_DATA(baseAddress + (uintptr_t)hex_digits_DATA2, 0, output, 16); 
    READ_ALIGNED_DATA(baseAddress + (uintptr_t)hex_digits_LITERAL, 0, output, 16);
}
void taskFunction(uintptr_t baseAddress, const char* input, char* output) {
 
    // Function call through address calculation
    uintptr_t testFunctionAddress = baseAddress + (uintptr_t)test;
    void (*testFunc)(char*) = (void (*)(char*))testFunctionAddress;
    testFunc(output);

    // Direct access to .text section data
    char hex_digits_TEXT[] = {0x41, 0x42, 0x43}; // .text
    memcpy(output, hex_digits_TEXT, sizeof(hex_digits_TEXT));

    // Example local variables
    int iAlpha = 10;
    int iBeta = 20;
    int iDelta = iAlpha + iBeta;

    // Accessing .rodata section
    uintptr_t hex_digits_address_RODATA = baseAddress + (uintptr_t)hex_digits_RODATA;
    READ_ALIGNED_DATA(hex_digits_address_RODATA, 0, output, 16);

    // Accessing .data section
    uintptr_t hex_digits_address_DATA = baseAddress + (uintptr_t)hex_digits_DATA;
    READ_ALIGNED_DATA(hex_digits_address_DATA, 0, output, 16);
    
    // Accessing .data section
    uintptr_t hex_digits_address_DATA2 = baseAddress + (uintptr_t)hex_digits_DATA2;
    READ_ALIGNED_DATA(hex_digits_address_DATA2, 0, output, 16);

    // Accessing .literal section
    uintptr_t hex_digits_address_LITERAL = baseAddress + (uintptr_t)hex_digits_LITERAL;
    READ_ALIGNED_DATA(hex_digits_address_LITERAL, 0, output, 16);

    // Function call through address calculation
    uintptr_t testFunctionAddress2 = baseAddress + (uintptr_t)test2;
    void (*testFunc2)(uintptr_t, char*) = (void (*)(uintptr_t, char*))testFunctionAddress2;
    testFunc2(baseAddress, output);
}

// Metadata
struct TaskMetadata __attribute__((section(".task_metadata"))) taskMetadata = {
    (uint32_t)&taskFunction,
    "AAAAAAAAAAAAAAA"
};
