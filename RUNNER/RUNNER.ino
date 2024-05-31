#include <Arduino.h>
#include <WiFi.h>
#include <string>
#include <HTTPClient.h>
#include "esp_heap_caps.h"
#include "FS.h"
#include "SPIFFS.h"

// Define the metadata structure stored at the start of the binary
// We will load the function address from it.
struct TaskMetadata {
  uint32_t taskFunctionAddress;
  char dummyText[16];
};

//
typedef void (*task_func_t)(const char*, char*);

void taskRunner(void* parameter) {
  // Cast the parameter back to the original buffer pointer
  uint8_t* programBuffer = reinterpret_cast<uint8_t*>(parameter);

  // Extract the task function address from the metadata
  TaskMetadata* metadata = reinterpret_cast<TaskMetadata*>(programBuffer);  
  Serial.printf("Task Function Address           : 0x%X\n", metadata->taskFunctionAddress);

  // Calculate the task function address by adding the offset to the buffer pointer
  uintptr_t taskFunctionAddress = reinterpret_cast<uintptr_t>(programBuffer) + reinterpret_cast<uintptr_t>(metadata->taskFunctionAddress);
  
  Serial.printf("Program buffer address          : 0x%X\n", programBuffer);
  Serial.printf("Calculated Task Function Address: 0x%X\n", taskFunctionAddress);

  task_func_t task = reinterpret_cast<task_func_t>(taskFunctionAddress);
 
  // Call the task function
  const char* input = "ESP32";
  char output[100];
  task(input, output);

  // Print the return value
  Serial.printf("Return value from task: %s\n", output);

  // REMEMBER TO FREE THE PROGRAM MEMORY !!!!!
  // void* program = heap_caps_malloc(fileSize, MALLOC_CAP_EXEC /*Memory must be able to run executable code */ || MALLOC_CAP_8BIT);
  vTaskDelete(NULL);
}


// Will load binary in chunks, convert and place in dynamically executable allocated buffer
// We can't have two buffers of a binary size at once, and we can't read from file directly to heap_caps_malloc allocated buffer
// Thus we need this intermediate step of temporary buffer.
void loadAndExecuteTask(const char* path) {

  Serial.println("Loading from SPIFFS");


  // Open the binary file to read the whole binary including metadata
  File taskFile = SPIFFS.open(path, FILE_READ);
  if (!taskFile) {
    Serial.println("Failed to open task file");
    return;
  }
  // Get the file size
  size_t fileSize = taskFile.size();
  Serial.println("BIN SIZE: " + String(fileSize));

  // Get the current free heap size
  size_t freeHeap = esp_get_free_heap_size();
  Serial.println("Free heap size: " + String(freeHeap));

  // Check if there is enough free heap memory
  if (freeHeap < fileSize) {
    Serial.println("Insufficient free heap memory");
    taskFile.close();
    return;
  }

  // Allocate memory for the task using heap_caps_malloc
  void* program = heap_caps_malloc(fileSize, MALLOC_CAP_EXEC /*Memory must be able to run executable code */ || MALLOC_CAP_8BIT);
  if (!program) {
    Serial.println("Failed to allocate memory");
    taskFile.close();
    return;
  }
  Serial.println("MALLOC OK");

  // Allocate a temporary buffer for reading a chunk of data
  const size_t chunkSize = 128;
  uint8_t* chunkBuffer = new uint8_t[chunkSize];
  if (!chunkBuffer) {
    Serial.println("Failed to allocate temporary buffer");
    heap_caps_free(program);
    taskFile.close();
    return;
  }

  // Read and process the file in chunks
  size_t bytesRead = 0;
  while (bytesRead < fileSize) {
    // Calculate the size of the current chunk
    size_t chunkBytes = min(chunkSize, fileSize - bytesRead);

    // Read a chunk from the file
    size_t chunkRead = taskFile.read(chunkBuffer, chunkBytes);
    if (chunkRead != chunkBytes) {
      Serial.println("Failed to read chunk from task file");
      heap_caps_free(program);
      delete[] chunkBuffer;
      taskFile.close();
      return;
    }

    // Convert and copy the chunk into the heap_caps_malloc buffer
    for (size_t i = 0; i < chunkRead; i += 4) {
      uint32_t inst = (chunkBuffer[i + 3] << 24) + (chunkBuffer[i + 2] << 16) + (chunkBuffer[i + 1] << 8) + chunkBuffer[i];
      ((uint32_t*)((uint8_t*)program + bytesRead))[i / 4] = inst;
    }

    // Update bytes read
    bytesRead += chunkRead;
  }

  // Close the file
  taskFile.close();

  // Free the temporary buffer
  delete[] chunkBuffer;

  Serial.println("SUBMITTING TASK");
  // Create a FreeRTOS task to execute the loaded code
  xTaskCreate(taskRunner, "TaskRunner", 8192, program, 1, NULL);
}


void setup() {
  Serial.begin(115200);
  /*
  WiFi.begin("WIFI", "WIFI12345667");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  */
  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  // Example to load and execute a task
  loadAndExecuteTask("/task.bin");
}

void loop() {
  Serial.println(".");
  // Main firmware loop to maintain the network connection and manage tasks
  delay(1000);
}
