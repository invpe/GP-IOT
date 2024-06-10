![image](https://github.com/invpe/GP-IOT/assets/106522950/b72d07c3-76c9-4583-8f40-53f39d463f7a)

# General Purpose Things - eXperiment

As part of an initiative to enhance the [GridShell](https://github.com/invpe/GridShell) project, i thought about executing binaries instead of scripts by dynamically loading and executing them on ESP32 nodes.

This repo called GP-IOT - General Purpose (Internet of) Things is my workbench, to get me to a place where i can do it issues free, easily and in secure manner.

General Purpose Things - draw inspiration from established distributed computing systems like BOINC, focusing on executing binaries on nodes. This repository introduces a Sketch named `Runner`, designed to dynamically load binaries from SPIFFS, and a `Task` source code that is an example binary. It locates the function address within the binary and executes it in a separate thread, showcasing a practical example of dynamic binary execution on IoT devices.

# Demo

The repository holds a ready-to-use solution, including `Runner` sketch and a simple `task`.

1. First upload the `Runner` sketch.
2. Then go to `task` and perform compilation, linkage, binary extraction and spiffs upload with `./string.sh`
3. Open up terminal and reboot your device, you should end up binary being exexuted :

```
[19:55:29:407] Connecting to WiFi...␍␊
[19:55:29:407] Connected to WiFi␍␊
[19:55:29:523] Loading from SPIFFS␍␊
[19:55:29:527] BIN SIZE: 152␍␊
[19:55:29:527] Free heap size: 201324␍␊
[19:55:29:527] MALLOC OK␍␊
[19:55:29:545] SUBMITTING TASK␍␊
[19:55:29:545] .␍␊
[19:55:29:545] Task Function Address           : 0x14␊
[19:55:29:545] Program buffer address          : 0x40092C9C␊
[19:55:29:545] Calculated Task Function Address: 0x40092CB0␊
[19:55:29:545] Return value from task: AAAAAAAAAA␊
```   

# Tweaks and improvements

- I have included WIFI connection there to see if i have a long running binary if it breaks the esp32. You can get rid of it.
- Need to find a way to encapsulate the execution with some kind of a `VM` or similar thing in the future.

# How

There are few conditions that have to be met in order for the example to work:

1. Binary we want to start needs to have a special `metadata` structure, that is placed at the start of the binary itself.
   This metadata holds a `uint32_t` address of the function we want to execute, in this case: `taskFunction`

```
struct TaskMetadata {    
    uint32_t taskFunctionAddress;       
    char dummyText[16];  
}; 
struct TaskMetadata __attribute__((section(".task_metadata"))) taskMetadata = {    
    (uint32_t)&taskFunction,
    "AAAAAAAAAAAAAAA"  // Initialize dummy text to see if our metadata is seen at the top of the binary
};
```

2. The compilation and later link process need to ensure this `metadata` is placed at the top of the binary, because `runner` sketch will pick this up and retrieve the `taskFunctionAddress`. I am using a template `.ld` file for that, might not be perfect but works.


```
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
```

3. The binary needs to have the same function definition as the `runner` so that we call it with the same arguments, and expect the same resunts (if anyhting is returned)

`typedef void (*task_func_t)(const char*, char*);`

`void taskFunction(const char* input, char* output)`


4. The binary once compiled must have a decent size, so that we can allocate the memory for it, there is a check performed in the `RUNNER` sketch, that bails out if execution is impossible due to memory issues.

```
  // Check if there is enough free heap memory
  if (freeHeap < fileSize) {
    Serial.println("Insufficient free heap memory");
    taskFile.close();
    return;
  }
```

5. The binary needs to be placed on SPIFFS, so ensure the SPIFFS is ready for use - that's handled by `Runner` sketch, which formats and initializes it properly.
6. The runner (once started) will then dynamically load and execute the binary in a separate thread to avoid blocking the main one.
   
   `xTaskCreate(taskRunner, "TaskRunner", 8192, program, 1, NULL);`
   

# ISSUES

## Variables Address space

The binary that is built is generated with a linker script that starts from `0x00`,
we store the metadata at the top, and then following the rest of the program including variables.

The address `0xE` of a char array returned is relative to the start of the raw binary, not an absolute address in the ESP32 memory space. When the raw binary is loaded into memory, `0xE` is not a valid memory address on the ESP32, which is why accessing it directly causes issues.

This means, that _any variable_ address will be in relation to the starting address we have given in the `.ld` file, i.e see example below:

```
#include <cstdlib>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../RUNNER/Common.h" 

static const char hex_digits[] = "0123456789ABCDEF";
 
void taskFunction(const char* input, uintptr_t *outputAddr) {  
     
     *outputAddr = (uintptr_t)hex_digits;
     
}
 

struct TaskMetadata __attribute__((section(".task_metadata"))) taskMetadata = {    
    (uint32_t)&taskFunction,
    "AAAAAAAAAAAAAAA"  // Initialize dummy text to see if our metadata is seen at the top of the binary
};
```

The address of `hex_digits` is `0xE` - if we try to use it, we can't (the program crashes) as the variable is not stored there for ESP32.
The `0xE` is in relation to the `0x00` - which is the starting point at which we build the whole raw binary.

```
[19:24:52:920] Program buffer address: 0x4008E580
[19:24:52:934] Metadata address: 0x4008E5A0
[19:24:52:934] Task Function Address in Metadata: 0x4
[19:24:52:934] Calculated Task Function Address: 0x4008E584
[19:24:52:934] Address of hex in taskFunction: 0xE  <<<<<<
```

Here is the use example:

```
#include <cstdlib>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../RUNNER/Common.h" 

static const char hex_digits[] = "0123456789ABCDEF";
 
void taskFunction(const char* input, uintptr_t *outputAddr, char *output) {  
          
     *outputAddr = (uintptr_t)hex_digits;

     // Will crash
     for(int x = 0; x < 13; x++)
     {
        output[x] = hex_digits[x];
     }
}
 

struct TaskMetadata __attribute__((section(".task_metadata"))) taskMetadata = {    
    (uint32_t)&taskFunction,
    "AAAAAAAAAAAAAAA"  // Initialize dummy text to see if our metadata is seen at the top of the binary
};

```

And the failure

```
[19:33:14:979] Size of TaskMetadata: 20
[19:33:14:985] Program buffer address: 0x4008E580
[19:33:14:992] Metadata address: 0x4008E5B4
[19:33:14:992] Task Function Address in Metadata: 0x4
[19:33:14:992] Calculated Task Function Address: 0x4008E584
[19:33:14:997] Guru Meditation Error: Core  1 panic'ed (LoadProhibited). Exception was unhandled.
```


To even better visualize the problem, let's take these examples.
First, let's look how memory is mapped in one of ESP32 `.ld` files:

```
// Memory segments defined in ESP32 .ld file
...
iram0_0_seg (RX) : org = 0x40080000, len = 0x20000
dram0_0_seg (RW) : org = 0x3FFB0000 + 0xdb5c,
..
```


Now let's create a simple raw binary, that will return the address of a variable `char` 
allocated on the `RUNNER` and passed to raw binary.

```
// Return the address of 'output' provided by RUNNER sketch
void taskFunction(const char* input, uintptr_t *outputAddr, char *output) {            
     *outputAddr = (uintptr_t)output;  
}
```

The returned output is:

```
[19:34:39:081] Program buffer address: 0x4008E580␊ << IRAM
[19:34:39:081] Metadata address: 0x4008E588␊ << IRAM
[19:34:39:089] Calculated Task Function Address: 0x4008E580␊ << IRAM
[19:34:39:093] Address of hex in taskFunction: 0x3FFB21B8␊ << `output` in DRAM
```

Now let's return the address of our `hex_digits` instead

```
// Return the address of `hex_digits` from the raw binary
void taskFunction(const char* input, uintptr_t *outputAddr, char *output) {            
     *outputAddr = (uintptr_t)hex_digits;  
}
```

And the output is:

```
[19:39:55:027] Program buffer address: 0x4008E580 << IRAM
[19:39:55:027] Metadata address: 0x4008E5A0 << IRAM
[19:39:55:035] Calculated Task Function Address: 0x4008E584 << IRAM
[19:39:55:039] Address of hex in taskFunction: 0xE << LOCAL TO BINARY
```




### Solution

Not known yet, as it might appear that the proper `.ld` script should be able to resolve the issue, by placing the variables at the proper READ ONLY segment in memory. TBD
