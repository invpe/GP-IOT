#!/bin/bash
clear
echo "Cleanup..."
rm -f string.bin
rm -f string.elf 
rm -f string.o

echo "COMPILE"
~/.arduino15/packages/m5stack/tools/xtensa-esp32-elf-gcc/esp-2021r2-patch5-8.4.0/bin/xtensa-esp32-elf-g++ -Os -c string.c -o string.o 

echo "LINK"
~/.arduino15/packages/m5stack/tools/xtensa-esp32-elf-gcc/esp-2021r2-patch5-8.4.0/bin/xtensa-esp32-elf-g++ -T string.ld -nostartfiles  string.o -o string.elf 

echo "ELF2BIN"
~/.arduino15/packages/m5stack/tools/xtensa-esp32-elf-gcc/esp-2021r2-patch5-8.4.0/bin/xtensa-esp32-elf-objcopy -O binary string.elf string.bin

echo "Copying"
cp string.bin ../RUNNER/data/task.bin
ls -lha string.bin
hexdump string.bin 

echo "Prepare data folder"
~/.arduino15/packages/m5stack/tools/mkspiffs/0.2.3/mkspiffs -c ../RUNNER/data/ -p 256 -b 4096 -s 0x160000 ./spiffs.bin

echo "Upload"
python3 ~/.arduino15/packages/m5stack/tools/esptool_py/4.5.1/esptool.py --chip esp32 --port "/dev/ttyUSB0" --baud 1500000  --before default_reset --after hard_reset write_flash  -z --flash_mode dio --flash_freq 80m --flash_size detect 0x290000 spiffs.bin

echo "No errors? You're ready to go"
