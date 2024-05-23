################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/GIT_WORKSPACE/STM32/Libraries/packet_handler/packetHandler.c 

OBJS += \
./packet_handler/packetHandler.o 

C_DEPS += \
./packet_handler/packetHandler.d 


# Each subdirectory must supply rules for building sources it contributes
packet_handler/packetHandler.o: D:/GIT_WORKSPACE/STM32/Libraries/packet_handler/packetHandler.c packet_handler/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F302xC -c -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I"D:/GIT_WORKSPACE/STM32/Libraries/ext_flash_w25q64" -I"D:/GIT_WORKSPACE/STM32/Libraries/int_flash" -I"D:/GIT_WORKSPACE/STM32/Libraries/packet_handler" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-packet_handler

clean-packet_handler:
	-$(RM) ./packet_handler/packetHandler.cyclo ./packet_handler/packetHandler.d ./packet_handler/packetHandler.o ./packet_handler/packetHandler.su

.PHONY: clean-packet_handler

