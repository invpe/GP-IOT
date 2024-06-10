![image](https://github.com/invpe/GP-IOT/assets/106522950/b72d07c3-76c9-4583-8f40-53f39d463f7a)

# General Purpose Things - eXperiment

As part of an initiative to enhance the [GridShell](https://github.com/invpe/GridShell) project, i thought about executing binaries instead of scripts by dynamically loading and executing them on ESP32 nodes.

This repo called GP-IOT - General Purpose (Internet of) Things is my workbench, to get me to a place where i can do it issues free, easily and in secure manner.

General Purpose Things - draw inspiration from established distributed computing systems like BOINC, focusing on executing binaries on nodes. This repository introduces a Sketch named `Runner`, designed to dynamically load binaries from SPIFFS, and a `Task` source code that is an example binary. It locates the function address within the binary and executes it in a separate thread, showcasing a practical example of dynamic binary execution on IoT devices.

# Demo

The repository holds a ready-to-use solution, including `Runner` sketch and a simple `task`.

1. First upload the `Runner` sketch.
2. Then go to `Example` and perform compilation, linkage, binary extraction and spiffs upload with `./task.sh`
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
 
# How

There are few conditions that have to be met in order for the example to work:

1. Binary we want to start needs to have a special `metadata` structure, that is placed at the END of the binary itself with the use of `.ld` script.
   
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

2. The compilation and later link process need to ensure this `metadata` is placed at the bottom of the binary, because `runner` sketch will pick this up and retrieve the `taskFunctionAddress`. I am using a template `.ld` file for that, might not be perfect but works.
 
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

6. The runner (once started) will then dynamically load and execute the binary, but it can be also done in a separate thread to avoid blocking the main one if necessary.
   
   `xTaskCreate(taskRunner, "TaskRunner", 8192, program, 1, NULL);`

7. The code is executed in IRAM, remember variables and addresses are relative to the start of the binary as there is no o/s initialization for this binary blob.

8. The CPU can access data via the data bus in a byte-, half-word-, or word-aligned manner. The CPU can also access data via the instruction bus, but only in a word-aligned manner; non-word-aligned access will cause a CPU exception.
9. And probably more.. ;-)
   

