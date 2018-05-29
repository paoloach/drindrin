cmake_minimum_required(VERSION 3.10)
project(firmware)


set(CMAKE_CXX_STANDARD 11)

# Flash size in megabits
# Valid values are same as for esptool.py - 2,4,8,16,32
SET(FLASH_SIZE, 16)

# Flash mode, valid values are same as for esptool.py - qio,qout,dio.dout
SET(FLASH_MODE, qio)

# Flash speed in MHz, valid values are same as for esptool.py - 80, 40, 26, 20
set(FLASH_SPEED, 40)

# set this to 0 if you don't need floating point support in printf/scanf
# this will save approx 14.5KB flash space and 448 bytes of statically allocated
# data RAM
#
# NB: Setting the value to 0 requires a recent esptool.py (Feb 2016 / commit ebf02c9)
set(PRINTF_SCANF_FLOAT_SUPPORT, 1)

set(EspOpenRtosPath /home/paolo/esp2866/esp-open-rtos)
file(GLOB LWIP_SRC ${EspOpenRtosPath}/lwip)
set(LWIP_INCLUDE ${EspOpenRtosPath}/lwip/lwip/src/include)
set(FREE_RTOS ${EspOpenRtosPath}/FreeRTOS/Source)

add_executable(firmware main.cpp wificfg.c FreeRTOSConfig.h ${LWIP_SRC})

target_include_directories(firmware PUBLIC
        ${LWIP_INCLUDE}
        ${EspOpenRtosPath}/core/include
        ${EspOpenRtosPath}/include
        ${FREE_RTOS}/include
        ${EspOpenRtosPath}/extras
        )