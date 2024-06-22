cmake_minimum_required(VERSION 3.29)

project(stm32-rtc-scheduler)
enable_language(C CXX ASM)

message("Build type: ${CMAKE_BUILD_TYPE}")

# Enable output of compile commands during generation
set(CMAKE_EXPORT_COMPILE_COMMANDS   ON)

set(CMAKE_C_STANDARD                11)
set(CMAKE_C_STANDARD_REQUIRED       ON)
set(CMAKE_C_EXTENSIONS              ON)
set(CMAKE_CXX_STANDARD              14)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_EXTENSIONS            ON)

set(PROJECT_DIR         ${CMAKE_SOURCE_DIR})
set(LINKER_SCRIPT       ${PROJECT_DIR}/projects/GCC/stm32l496xx_flash.ld)
set(TARGET_MAP_OUTPUT   ${CMAKE_PROJECT_NAME}.map)
set(TARGET_BIN_OUTPUT   ${CMAKE_PROJECT_NAME}.bin)
set(TARGET_HEX_OUTPUT   ${CMAKE_PROJECT_NAME}.hex)

set(DEBUG_FLAGS
    -Og
    -g
)

set(RELEASE_FLAGS
    -O2
)

set(MCU_FLAGS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

set(COMMON_FLAGS
    -Wall
    -pedantic
    -fdata-sections
    -ffunction-sections
)

set(ASM_FLAGS
    -x assembler-with-cpp
)

set(CC_FLAGS
    -MMD
    -MP
)

set(LINKER_FLAGS
    -T${LINKER_SCRIPT}
    -specs=nano.specs
    -specs=nosys.specs
    -lc
    -lm
    -lnosys
    -Wl,-Map=${TARGET_MAP_OUTPUT},--cref
    -Wl,--gc-sections
)

set(DEFINES
    STM32L496xx
    USE_HAL_DRIVER
    USE_FULL_ASSERT
)

set(INCLUDES
    ${PROJECT_DIR}/include
    ${PROJECT_DIR}/drivers/CMSIS/Include
    ${PROJECT_DIR}/drivers/CMSIS/Device/ST/STM32L4xx/Include
    ${PROJECT_DIR}/drivers/STM32L4xx_HAL_Driver/Inc
    ${PROJECT_DIR}/lib/FreeRTOS/include
    ${PROJECT_DIR}/lib/FreeRTOS/portable/GCC/ARM_CM4F
)

file(GLOB SOURCES
    ${PROJECT_DIR}/source/*.c
    ${PROJECT_DIR}/drivers/STM32L4xx_HAL_Driver/Src/*.c
    ${PROJECT_DIR}/lib/FreeRTOS/*.c
    ${PROJECT_DIR}/lib/FreeRTOS/portable/GCC/ARM_CM4F/*.c
)

set(SOURCES
    ${SOURCES}
    lib/FreeRTOS/portable/MemMang/heap_1.c
    projects/GCC/startup_stm32l496xx.s
)

set(CMAKE_ASM_FLAGS         ${ASM_FLAGS} ${MCU_FLAGS} ${COMMON_FLAGS})
set(CMAKE_ASM_FLAGS_DEBUG   ${DEBUG_FLAGS})
set(CMAKE_ASM_FLAGS_RELEASE ${RELEASE_FLAGS})

set(CMAKE_C_FLAGS           ${CC_FLAGS} ${MCU_FLAGS} ${COMMON_FLAGS})
set(CMAKE_C_FLAGS_DEBUG     ${DEBUG_FLAGS})
set(CMAKE_C_FLAGS_RELEASE   ${RELEASE_FLAGS})

set(CMAKE_CXX_FLAGS         ${CC_FLAGS} ${MCU_FLAGS} ${COMMON_FLAGS})
set(CMAKE_CXX_FLAGS_DEBUG   ${DEBUG_FLAGS})
set(CMAKE_CXX_FLAGS_RELEASE ${RELEASE_FLAGS})

set(CMAKE_EXE_LINKER_FLAGS  ${MCU_FLAGS} ${LINKER_FLAGS})

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS   "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

string(REPLACE ";" " " CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS}")
string(REPLACE ";" " " CMAKE_ASM_FLAGS_DEBUG "${CMAKE_ASM_FLAGS_DEBUG}")
string(REPLACE ";" " " CMAKE_ASM_FLAGS_RELEASE "${CMAKE_ASM_FLAGS_RELEASE}")

string(REPLACE ";" " " CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
string(REPLACE ";" " " CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
string(REPLACE ";" " " CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REPLACE ";" " " CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
string(REPLACE ";" " " CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

string(REPLACE ";" " " CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

set(COMPILE_OUTPUT_FOLDER
    ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${CMAKE_PROJECT_NAME}.dir
)

# Add extra flags
foreach(source_file ${SOURCES})
    string(REPLACE "${PROJECT_DIR}/" "" source_file ${source_file})
    set(listing_file
        ${COMPILE_OUTPUT_FOLDER}/${source_file}.lst
    )
    set_property(SOURCE ${source_file} APPEND PROPERTY COMPILE_OPTIONS
        -Wa,-a,-ad,-alms=${listing_file}
    )
endforeach()

# Specify executables
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

# Specify includes
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${INCLUDES})

# Specify defines
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE ${DEFINES})

# Generate size information
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${CMAKE_PROJECT_NAME}>
)

# Generate binary output
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${TARGET_BIN_OUTPUT}
    COMMAND ${CMAKE_COMMAND} -E echo "Creating binary output: ${TARGET_BIN_OUTPUT}"
)
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES
    ${TARGET_BIN_OUTPUT})

# Generate hex output
add_custom_command(TARGET ${CMAKE_PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${CMAKE_PROJECT_NAME}> ${TARGET_HEX_OUTPUT}
    COMMAND ${CMAKE_COMMAND} -E echo "Creating hex output: ${TARGET_HEX_OUTPUT}"
)

# Specify additional clean files
set_property(TARGET ${CMAKE_PROJECT_NAME} APPEND PROPERTY ADDITIONAL_CLEAN_FILES
    ${TARGET_MAP_OUTPUT}
    ${TARGET_BIN_OUTPUT}
    ${TARGET_HEX_OUTPUT}
    ${COMPILE_OUTPUT_FOLDER}
)
