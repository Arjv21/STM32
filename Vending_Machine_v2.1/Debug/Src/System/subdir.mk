################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/System/adc.c \
../Src/System/common.c \
../Src/System/gpio.c \
../Src/System/spi.c \
../Src/System/sys_peri_clock.c \
../Src/System/timer.c \
../Src/System/uart.c 

OBJS += \
./Src/System/adc.o \
./Src/System/common.o \
./Src/System/gpio.o \
./Src/System/spi.o \
./Src/System/sys_peri_clock.o \
./Src/System/timer.o \
./Src/System/uart.o 

C_DEPS += \
./Src/System/adc.d \
./Src/System/common.d \
./Src/System/gpio.d \
./Src/System/spi.d \
./Src/System/sys_peri_clock.d \
./Src/System/timer.d \
./Src/System/uart.d 


# Each subdirectory must supply rules for building sources it contributes
Src/System/%.o Src/System/%.su Src/System/%.cyclo: ../Src/System/%.c Src/System/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F302xC -c -I../Inc -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/chip_headers/CMSIS/Device/ST/STM32F3xx/Include" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/chip_headers/CMSIS/Include" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/A7672" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/ExtFlashMem" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/JSON" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/osKernel" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/System" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/Misc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-System

clean-Src-2f-System:
	-$(RM) ./Src/System/adc.cyclo ./Src/System/adc.d ./Src/System/adc.o ./Src/System/adc.su ./Src/System/common.cyclo ./Src/System/common.d ./Src/System/common.o ./Src/System/common.su ./Src/System/gpio.cyclo ./Src/System/gpio.d ./Src/System/gpio.o ./Src/System/gpio.su ./Src/System/spi.cyclo ./Src/System/spi.d ./Src/System/spi.o ./Src/System/spi.su ./Src/System/sys_peri_clock.cyclo ./Src/System/sys_peri_clock.d ./Src/System/sys_peri_clock.o ./Src/System/sys_peri_clock.su ./Src/System/timer.cyclo ./Src/System/timer.d ./Src/System/timer.o ./Src/System/timer.su ./Src/System/uart.cyclo ./Src/System/uart.d ./Src/System/uart.o ./Src/System/uart.su

.PHONY: clean-Src-2f-System

