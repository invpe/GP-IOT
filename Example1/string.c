#include <unistd.h>
#include <stdio.h>
#include <string.h>  
#include "../RUNNER/Common.h"

// Define the task function
void taskFunction(const char* input, char* output) 
{
    
    memset(output, 0x41, 10); 
 
}

/////////////////////////////////////////////////////
// DO NOT TOUCH
/////////////////////////////////////////////////////
struct TaskMetadata __attribute__((section(".task_metadata"))) taskMetadata = {    
    (uint32_t)&taskFunction,
    "AAAAAAAAAAAAAAA"  // Initialize dummy text to see if our metadata is seen at the top of the binary
};
