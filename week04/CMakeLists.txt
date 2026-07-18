cmake_minimum_required(VERSION 3.20)

# Toolchain ARM phải được thiết lập trước project()
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(
        CMAKE_TOOLCHAIN_FILE
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/arm-gcc.cmake"
        CACHE FILEPATH "ARM GCC toolchain file"
    )
endif()

project(week04 LANGUAGES C ASM)

# Chọn Task cần build
set(HW_TASK "3" CACHE STRING "Task to build: 3, 4, or 5")
set_property(CACHE HW_TASK PROPERTY STRINGS 3 4 5)

if(NOT HW_TASK MATCHES "^[345]$")
    message(FATAL_ERROR "HW_TASK must be 3, 4, or 5")
endif()

message(STATUS "Building Task ${HW_TASK}")
message(STATUS "Source file: Task${HW_TASK}/main.c")

set(TARGET_NAME week04)

# Công cụ tạo HEX/BIN
find_program(ARM_OBJCOPY arm-none-eabi-objcopy REQUIRED)
find_program(ARM_SIZE arm-none-eabi-size REQUIRED)

set(MCU_FLAGS
    -mcpu=cortex-m3
    -mthumb
)

set(PROJECT_SOURCES
    Task${HW_TASK}/main.c

    Core/Src/system_stm32f1xx.c
    Core/Src/startup_stm32f103xb.s
    Core/Src/stm32f1xx_it.c

    Drivers/STM32F1xx_HAL_Driver/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
    Drivers/STM32F1xx_HAL_Driver/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
    Drivers/STM32F1xx_HAL_Driver/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
    Drivers/STM32F1xx_HAL_Driver/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
)

add_executable(${TARGET_NAME} ${PROJECT_SOURCES})

target_compile_definitions(${TARGET_NAME} PRIVATE
    USE_HAL_DRIVER
    STM32F103xB
)

target_include_directories(${TARGET_NAME} PRIVATE
    Core/Inc
    Drivers/STM32F1xx_HAL_Driver/STM32F1xx_HAL_Driver/Inc
    Drivers/CMSIS/Include
    Drivers/CMSIS/Device/ST/STM32F1xx/Include
)

target_compile_options(${TARGET_NAME} PRIVATE
    ${MCU_FLAGS}

    -Wall
    -Wextra

    -ffunction-sections
    -fdata-sections
)

target_link_options(${TARGET_NAME} PRIVATE
    ${MCU_FLAGS}

    -T${CMAKE_CURRENT_SOURCE_DIR}/stm32f103xb_flash.ld

    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.map

    --specs=nano.specs
    --specs=nosys.specs
)

set_target_properties(${TARGET_NAME} PROPERTIES
    SUFFIX ".elf"
)

add_custom_command(
    TARGET ${TARGET_NAME}
    POST_BUILD

    COMMAND "${ARM_OBJCOPY}"
        -O ihex
        "$<TARGET_FILE:${TARGET_NAME}>"
        "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.hex"

    COMMAND "${ARM_OBJCOPY}"
        -O binary
        "$<TARGET_FILE:${TARGET_NAME}>"
        "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.bin"

    COMMAND "${ARM_SIZE}"
        "$<TARGET_FILE:${TARGET_NAME}>"

    COMMENT "Generating HEX and BIN files"
    VERBATIM
)