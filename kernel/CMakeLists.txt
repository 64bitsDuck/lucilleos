cmake_minimum_required(VERSION 3.20)

# Declare the executable target for the kernel
add_executable(kernel
    src/main.c
    src/limine.h
)

# Specify the linker script
target_link_options(kernel PRIVATE
    -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
    -nostdlib
    -static
    -no-pie
)

# Allow inclusion of files from the src/ directory
target_include_directories(kernel PRIVATE src)