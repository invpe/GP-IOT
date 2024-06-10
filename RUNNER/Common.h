struct TaskMetadata {    
    uint32_t taskFunctionAddress;       
    char dummyText[16];  
}; 
typedef void (*task_func_t)(uintptr_t,const char*,uintptr_t*,char*);
