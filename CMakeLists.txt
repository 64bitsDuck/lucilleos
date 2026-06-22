cmake_minimum_required(VERSION 3.20)
project(LucilleOS LANGUAGES C ASM)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

add_compile_options(
    -ffreestanding
    -nostdlib
    -fno-builtin
    -fno-stack-protector
    -fno-pic
    -fno-pie
    -mno-red-zone
    -mno-mmx
    -mno-sse
    -mno-sse2
    -Wall
    -Wextra
    -Werror
)

add_subdirectory(kernel)

# --- ISO and Bootloader Automation ---

set(ISO_DIR ${CMAKE_BINARY_DIR}/iso)

# Custom target to assemble and build the bootable ISO
add_custom_target(iso
    DEPENDS kernel
    COMMAND ${CMAKE_COMMAND} -E make_directory ${ISO_DIR}/boot
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:kernel> ${ISO_DIR}/boot/kernel
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/limine.conf ${ISO_DIR}/boot/limine.conf
    
    # Download Limine boot files if missing
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/limine
    COMMAND test -f ${CMAKE_BINARY_DIR}/limine/limine-bios.sys || curl -o ${CMAKE_BINARY_DIR}/limine/limine-bios.sys https://raw.githubusercontent.com/limine-bootloader/limine/v7.x-binary/limine-bios.sys
    COMMAND test -f ${CMAKE_BINARY_DIR}/limine/limine-cd.bin || curl -o ${CMAKE_BINARY_DIR}/limine/limine-cd.bin https://raw.githubusercontent.com/limine-bootloader/limine/v7.x-binary/limine-cd.bin
    
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/limine/limine-bios.sys ${ISO_DIR}/boot/
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/limine/limine-cd.bin ${ISO_DIR}/boot/
    
    # Generate the ISO using xorriso
    COMMAND xorriso -as mkisofs -b boot/limine-cd.bin
                    -no-emul-boot -boot-load-size 4 -boot-info-table
                    ${ISO_DIR} -o ${CMAKE_BINARY_DIR}/lucilleos.iso
    COMMENT "Generating bootable ISO image"
)

# Custom target to run the OS in QEMU
add_custom_target(run
    DEPENDS iso
    COMMAND qemu-system-x86_64 -M q35 -m 256M -cdrom ${CMAKE_BINARY_DIR}/lucilleos.iso -serial stdio
    COMMENT "Running LucilleOS in QEMU"
)