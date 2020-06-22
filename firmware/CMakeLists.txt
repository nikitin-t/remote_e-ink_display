# Project name
PROJECT(remote_e-ink_display)

CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

ENABLE_LANGUAGE(ASM)

# Project sources
SET(PROJECT_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/main.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/crc.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/app.c
)

# Startup sources
SET(STM32_STARTUP ${CMAKE_CURRENT_SOURCE_DIR}/startup_stm32l053xx.s)
# System source
SET(STM32_SYSTEM_SOURCE 
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/system_stm32l0xx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/spi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/usart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/stm32l0xx_it.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/stm32l0xx_hal_msp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/syscalls.c
    
)

# BSP sources
SET(BSP_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/BSP/STM32L0538-Discovery/stm32l0538_discovery_epd.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/BSP/STM32L0538-Discovery/stm32l0538_discovery.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/BSP/Components/gde021a1/gde021a1.c
)

# HAL sources
SET(HAL_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_spi.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_i2c.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_i2c_ex.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_rcc_ex.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash_ramfunc.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_flash_ex.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_gpio.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_dma.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_pwr_ex.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_cortex.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_tim.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_tim_ex.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_uart.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_uart_ex.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_crc.c
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Src/stm32l0xx_hal_crc_ex.c
)

# Configuring linker script for our target
SET(CMAKE_EXE_LINKER_FLAGS "-T${CMAKE_CURRENT_SOURCE_DIR}/STM32L053C8Tx_FLASH.ld ${CMAKE_EXE_LINKER_FLAGS}")

INCLUDE_DIRECTORIES(
  ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/BSP/STM32L0538-Discovery
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/BSP/Components/Common
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/BSP/Components/gde021a1
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Inc
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32L0xx_HAL_Driver/Inc/Legacy
  ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32L0xx/Include
)

ADD_DEFINITIONS(
  -DUSE_HAL_DRIVER 
  -DSTM32L053xx
)

# Compile executable (*.elf)
ADD_EXECUTABLE(${CMAKE_PROJECT_NAME}.elf ${PROJECT_SOURCES} ${STM32_STARTUP} ${STM32_SYSTEM_SOURCE} ${HAL_SOURCES} ${BSP_SOURCES})

# Convert elf to bin and hex
ADD_CUSTOM_COMMAND(TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD COMMAND ${CMAKE_OBJCOPY} ARGS -Oihex ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.hex)
ADD_CUSTOM_COMMAND(TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD COMMAND ${CMAKE_OBJCOPY} ARGS -Obinary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin)

