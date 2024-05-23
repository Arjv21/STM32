################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/GIT_WORKSPACE/STM32/Libraries/ext_flash_w25q64/ext_flash_w25q64.c 

OBJS += \
./ext_flash_w25q64/ext_flash_w25q64.o 

C_DEPS += \
./ext_flash_w25q64/ext_flash_w25q64.d 


# Each subdirectory must supply rules for building sources it contributes
ext_flash_w25q64/ext_flash_w25q64.o: D:/GIT_WORKSPACE/STM32/Libraries/ext_flash_w25q64/ext_flash_w25q64.c ext_flash_w25q64/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F302xC -c -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I"D:/GIT_WORKSPACE/STM32/Libraries/ext_flash_w25q64" -I"D:/GIT_WORKSPACE/STM32/Libraries/int_flash" -I"D:/GIT_WORKSPACE/STM32/Libraries/packet_handler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ext_flash_w25q64

clean-ext_flash_w25q64:
	-$(RM) ./ext_flash_w25q64/ext_flash_w25q64.cyclo ./ext_flash_w25q64/ext_flash_w25q64.d ./ext_flash_w25q64/ext_flash_w25q64.o ./ext_flash_w25q64/ext_flash_w25q64.su

.PHONY: clean-ext_flash_w25q64

