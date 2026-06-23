# Toolchain definitions
ASM = nasm
CC = gcc
LD = ld
QEMU = qemu-system-x86_64

# Compilation flags
CFLAGS = -ffreestanding -O2 -m64 -mno-red-zone -Wall -Wextra
LDFLAGS = -T kernel/linker.ld

# Directories
BUILD_DIR = build
BOOT_SRC = boot/boot.asm
KERNEL_ASM_SRC = kernel/src/kernel.asm
KERNEL_C_SRC = kernel/src/main.c

# Target files
BOOT_BIN = $(BUILD_DIR)/boot.bin
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
DISK_IMG = $(BUILD_DIR)/lucilleos.img

# Object files
OBJ = $(BUILD_DIR)/kernel_asm.o $(BUILD_DIR)/kernel_c.o

.PHONY: all clean run

all: $(DISK_IMG)

$(DISK_IMG): $(BOOT_BIN) $(KERNEL_BIN)
	@mkdir -p $(BUILD_DIR)
	cat $(BOOT_BIN) $(KERNEL_BIN) > $(DISK_IMG)
	truncate -s 10M $(DISK_IMG)

$(BOOT_BIN): $(BOOT_SRC)
	@mkdir -p $(BUILD_DIR)
	$(ASM) -f bin $(BOOT_SRC) -o $(BOOT_BIN)

$(BUILD_DIR)/kernel_asm.o: $(KERNEL_ASM_SRC)
	@mkdir -p $(BUILD_DIR)
	$(ASM) -f elf64 $(KERNEL_ASM_SRC) -o $(BUILD_DIR)/kernel_asm.o

$(BUILD_DIR)/kernel_c.o: $(KERNEL_C_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(KERNEL_C_SRC) -o $(BUILD_DIR)/kernel_c.o

$(KERNEL_BIN): $(OBJ)
	$(LD) $(LDFLAGS) -o $(KERNEL_BIN) $(OBJ)

run: $(DISK_IMG)
	$(QEMU) -drive format=raw,file=$(DISK_IMG)

clean:
	rm -rf $(BUILD_DIR)