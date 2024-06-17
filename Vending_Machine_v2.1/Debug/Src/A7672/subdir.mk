################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/A7672/ATCMD_A7672_Callbacks.c \
../Src/A7672/ATCMD_A7672_Commands.c \
../Src/A7672/ATCMD_Core.c \
../Src/A7672/ATCMD_Queue_Mgmt.c 

OBJS += \
./Src/A7672/ATCMD_A7672_Callbacks.o \
./Src/A7672/ATCMD_A7672_Commands.o \
./Src/A7672/ATCMD_Core.o \
./Src/A7672/ATCMD_Queue_Mgmt.o 

C_DEPS += \
./Src/A7672/ATCMD_A7672_Callbacks.d \
./Src/A7672/ATCMD_A7672_Commands.d \
./Src/A7672/ATCMD_Core.d \
./Src/A7672/ATCMD_Queue_Mgmt.d 


# Each subdirectory must supply rules for building sources it contributes
Src/A7672/%.o Src/A7672/%.su Src/A7672/%.cyclo: ../Src/A7672/%.c Src/A7672/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F302xC -c -I../Inc -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/chip_headers/CMSIS/Device/ST/STM32F3xx/Include" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/chip_headers/CMSIS/Include" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/A7672" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/ExtFlashMem" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/JSON" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/osKernel" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/System" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/Misc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-A7672

clean-Src-2f-A7672:
	-$(RM) ./Src/A7672/ATCMD_A7672_Callbacks.cyclo ./Src/A7672/ATCMD_A7672_Callbacks.d ./Src/A7672/ATCMD_A7672_Callbacks.o ./Src/A7672/ATCMD_A7672_Callbacks.su ./Src/A7672/ATCMD_A7672_Commands.cyclo ./Src/A7672/ATCMD_A7672_Commands.d ./Src/A7672/ATCMD_A7672_Commands.o ./Src/A7672/ATCMD_A7672_Commands.su ./Src/A7672/ATCMD_Core.cyclo ./Src/A7672/ATCMD_Core.d ./Src/A7672/ATCMD_Core.o ./Src/A7672/ATCMD_Core.su ./Src/A7672/ATCMD_Queue_Mgmt.cyclo ./Src/A7672/ATCMD_Queue_Mgmt.d ./Src/A7672/ATCMD_Queue_Mgmt.o ./Src/A7672/ATCMD_Queue_Mgmt.su

.PHONY: clean-Src-2f-A7672

