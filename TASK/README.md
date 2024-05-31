# string.c
This is the example task that gets compiled, linked and uploaded as binary to the SPIFFS of the RUNNER sketch.
It's function is to:

1. Store the function address in the `TaskMetadata.taskFunctionAddress`
2. Store few `AAAAAAAAAAAAAAA` characters in `TaskMetadata.dummyText` - so we can easily see using hexdump that our metadata is placed at the start of binary, which is crucial.

```
$ hexdump string.bin
0000000 0014 0000 4141 4141 4141 4141 4141 4141 <<< METADATA IS AT THE TOP
0000010 4141 0041 4136 0c00 4cac ad1b e503 0002
0000020 f01d 0000 7600 0494 4532 1b00 1d55 00f0
0000030 84b6 32f1 0045 551b 440b 6517 b628 e484
0000040 5532 2b00 4255 fec4 0686 0000 2136 3000
0000050 7430 7380 7011 2033 7300 7011 2033 025d
0000060 e207 17cc d6e2 7440 7641 0a97 0539 1539
0000070 2539 3539 c552 3710 0664 0539 1539 c552
0000080 2708 0364 0539 554b 6417 3204 0055 552b
0000090 6407 3202 0045 f01d                    
0000098
```
3. Have `taskFunction` that will get executed

# string.sh

This takes care of compilation, linkage and upload of the binary to SPIFFS

# string.ld

A template used for linker to place metadata at the top of the binary. This is a dirty one, not an expert in these things.
