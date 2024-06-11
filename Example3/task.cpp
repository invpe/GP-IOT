#include <stdint.h>
#include <string.h>
#include <cstdio>
#include "../RUNNER/Common.h"
#define GET_VAR(base, var) (*(typeof(var) *)((base) + (uintptr_t)(&(var)) - (uintptr_t)0x00))
#define SET_VAR(base, var, value) (*(typeof(var) *)((base) + (uintptr_t)(&(var)) - (uintptr_t)0x00)) = (value)
#define READ_ALIGNED_BYTES(base, arr, output, length) do {                   \
    union {                                                                  \
        uint32_t word;                                                       \
        char bytes[4];                                                       \
    } data;                                                                  \
    uintptr_t arr_address = (base) + (uintptr_t)(arr);                       \
    for (int i = 0; i < (length); i += 4) {                                  \
        data.word = *((uint32_t*)(arr_address + i));                         \
        for (int j = 0; j < 4 && (i + j) < (length); ++j) {                  \
            (output)[i + j] = data.bytes[j];                                 \
        }                                                                    \
    }                                                                        \
} while(0)

uintptr_t uipBaseAddress; 
const char hex_digits[]  = "0123456789ABCDEF";
int iVariable = 0; 
uint8_t uiVariable8 = 0; 
uint16_t uiVariable16 = 0; 
uint32_t uiVariable32 = 0; 

void taskFunction(uintptr_t baseAddress, const char* input,char *output) {
    
    // Test setting variable
    SET_VAR(baseAddress, uipBaseAddress, baseAddress); // Works
    SET_VAR(baseAddress, iVariable, 100);  // Works
    SET_VAR(baseAddress, uiVariable32, 100); // Works

    //SET_VAR(baseAddress, uiVariable16, 100); // Does not work due to alignment
    //SET_VAR(baseAddress, uiVariable8, 100);  // Does not work due to alignment

    // Test getting variable
    uint8_t uiA = GET_VAR(baseAddress, uiVariable8);  // Works
    uint16_t uiB = GET_VAR(baseAddress, uiVariable16); // Works
    uint32_t uiC = GET_VAR(baseAddress, uiVariable32); // Works
    int iV = GET_VAR(baseAddress, iVariable);    // Works

    // Read from the hex_digits array using the macro
    READ_ALIGNED_BYTES(baseAddress, hex_digits, output, 16);
    output[16] = '\0'; // Null-terminate the output string 

}

// Metadata
struct TaskMetadata __attribute__((section(".task_metadata"))) taskMetadata = {    
    (uint32_t)&taskFunction,
    "AAAAAAAAAAAAAAA"
};
