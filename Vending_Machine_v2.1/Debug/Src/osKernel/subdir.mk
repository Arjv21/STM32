################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/osKernel/osKernel.c 

OBJS += \
./Src/osKernel/osKernel.o 

C_DEPS += \
./Src/osKernel/osKernel.d 


# Each subdirectory must supply rules for building sources it contributes
Src/osKernel/%.o Src/osKernel/%.su Src/osKernel/%.cyclo: ../Src/osKernel/%.c Src/osKernel/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F302xC -c -I../Inc -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/chip_headers/CMSIS/Device/ST/STM32F3xx/Include" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/chip_headers/CMSIS/Include" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/A7672" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/ExtFlashMem" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/JSON" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/osKernel" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/System" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/Misc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-osKernel

clean-Src-2f-osKernel:
	-$(RM) ./Src/osKernel/osKernel.cyclo ./Src/osKernel/osKernel.d ./Src/osKernel/osKernel.o ./Src/osKernel/osKernel.su

.PHONY: clean-Src-2f-osKernel

