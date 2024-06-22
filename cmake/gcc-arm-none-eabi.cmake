# the name of the target operating system
set(CMAKE_SYSTEM_NAME       Generic)
set(CMAKE_SYSTEM_VERSION    1)
set(CMAKE_SYSTEM_PROCESSOR  arm)

list(APPEND CMAKE_PREFIX_PATH
    "C:/gcc/10-2020-q4/bin"
)

set(CROSS_COMPILER_PREFIX           arm-none-eabi-)
find_program(CMAKE_C_COMPILER       ${CROSS_COMPILER_PREFIX}gcc)
find_program(CMAKE_CXX_COMPILER     ${CROSS_COMPILER_PREFIX}g++)
find_program(CMAKE_ASM_COMPILER     ${CROSS_COMPILER_PREFIX}gcc)
find_program(CMAKE_OBJCOPY          ${CROSS_COMPILER_PREFIX}objcopy)
find_program(CMAKE_OBJDUMP          ${CROSS_COMPILER_PREFIX}objdump)
find_program(CMAKE_SIZE             ${CROSS_COMPILER_PREFIX}size)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# search for program/library/include in the build host directories
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf)
set(CMAKE_EXECUTABLE_SUFFIX_C   .elf)
set(CMAKE_EXECUTABLE_SUFFIX_CXX .elf)
