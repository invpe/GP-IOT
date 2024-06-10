#!/bin/bash
clear

# Cleanup previous builds
echo "Cleanup..."
rm -f task.bin task.elf task.o
 
echo "COMPILE TASK"
~/.arduino15/packages/m5stack/tools/xtensa-esp32-elf-gcc/esp-2021r2-patch5-8.4.0/bin/xtensa-esp32-elf-g++ -fPIC -Os -c task.cpp -o task.o

echo "LINK"
~/.arduino15/packages/m5stack/tools/xtensa-esp32-elf-gcc/esp-2021r2-patch5-8.4.0/bin/xtensa-esp32-elf-g++ \
-fPIC \
-T ../Common/linker.ld \
task.o \
-o task.elf \
-nostartfiles

# Strip debug symbols
echo "STRIP"
~/.arduino15/packages/m5stack/tools/xtensa-esp32-elf-gcc/esp-2021r2-patch5-8.4.0/bin/xtensa-esp32-elf-strip task.elf

# Check the ELF file for sections
echo "ELF Sections"
~/.arduino15/packages/m5stack/tools/xtensa-esp32-elf-gcc/esp-2021r2-patch5-8.4.0/bin/xtensa-esp32-elf-objdump -h task.elf

# Convert ELF to binary
echo "ELF2BIN"
~/.arduino15/packages/m5stack/tools/xtensa-esp32-elf-gcc/esp-2021r2-patch5-8.4.0/bin/xtensa-esp32-elf-objcopy -O binary task.elf task.bin

# Show
ls -lha task.bin
hexdump -C task.bin 


# Verify the binary content
echo "Copying"
cp task.bin ../RUNNER/data/task.bin

echo "Prepare data folder"
~/.arduino15/packages/m5stack/tools/mkspiffs/0.2.3/mkspiffs -c ../RUNNER/data/ -p 256 -b 4096 -s 0x160000 ./spiffs.bin

echo "Upload"
python3 ~/.arduino15/packages/m5stack/tools/esptool_py/4.5.1/esptool.py --chip esp32 --port "/dev/ttyUSB0" --baud 1500000 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect 0x290000 spiffs.bin
