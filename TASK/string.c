#include <unistd.h>
#include <stdio.h>
#include <string.h>  
// Define the task function
void taskFunction(const char* input, char* output) 
{
    
    memset(output, 0x41, 10); 
 
}

/////////////////////////////////////////////////////
// DO NOT TOUCH
/////////////////////////////////////////////////////
// This is stored at the start of the compiled binary
// Runner will load the address of our task function
struct TaskMetadata {    
    uint32_t taskFunctionAddress;       
    char dummyText[16];  
}; 
struct TaskMetadata __attribute__((section(".task_metadata"))) taskMetadata = {    
    (uint32_t)&taskFunction,
    "AAAAAAAAAAAAAAA"  // Initialize dummy text to see if our metadata is seen at the top of the binary
};
