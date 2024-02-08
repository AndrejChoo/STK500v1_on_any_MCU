################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/delay.c \
../src/main.c \
../src/progger.c \
../src/rcc.c \
../src/spi.c \
../src/uart.c 

OBJS += \
./src/delay.o \
./src/main.o \
./src/progger.o \
./src/rcc.o \
./src/spi.o \
./src/uart.o 

C_DEPS += \
./src/delay.d \
./src/main.d \
./src/progger.d \
./src/rcc.d \
./src/spi.d \
./src/uart.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32F0 -DSTM32F030K6Tx -DDEBUG -I"D:/STM32_projects/AVR_ISP_UART/CMSIS/core" -I"D:/STM32_projects/AVR_ISP_UART/CMSIS/device" -I"D:/STM32_projects/AVR_ISP_UART/inc" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


