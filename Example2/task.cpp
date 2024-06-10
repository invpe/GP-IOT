#include <cstdlib>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../RUNNER/Common.h" 

/*
The CPU can access data via the data bus in a byte-, half-word-, or word-aligned manner. The CPU can also access data via the instruction bus, but only in a word-aligned manner; non-word-aligned access will cause a CPU exception
*/
static const char hex_digits[] __attribute__((aligned(4))) = "0123456789ABCDEF";

// Function we will execute
void taskFunction(uintptr_t baseAddress, const char* input, uintptr_t* outputAddr, char *output) 
{

    // Needed to access the data in word-aligned manner
    union {
        char bytes[4];
        uint32_t word;
    } data;

    // Report the address of the hex digits
    *outputAddr = (uintptr_t)hex_digits;

    // Calculate the address of the hex digits from the start of binary
    uintptr_t hex_digits_address = baseAddress + (uintptr_t)hex_digits;

    // Read and return back to RUNNER
    for (int i = 0; i < 16; i += 4) {
        data.word = *((uint32_t*)(hex_digits_address + i));
        for (int j = 0; j < 4 && (i + j) < 16; ++j) {
            output[i + j] = data.bytes[j];
        }
    }
    output[16] = '\0'; // Null-terminate the output string
}

// Metadata
struct TaskMetadata __attribute__((section(".task_metadata"))) taskMetadata = {    
    (uint32_t)&taskFunction,
    "AAAAAAAAAAAAAAA"  // Initialize dummy text to see if our metadata is seen at the top of the binary
};
