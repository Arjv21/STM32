################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/JSON/JSONParser.c \
../Src/JSON/JSONWriter.c 

OBJS += \
./Src/JSON/JSONParser.o \
./Src/JSON/JSONWriter.o 

C_DEPS += \
./Src/JSON/JSONParser.d \
./Src/JSON/JSONWriter.d 


# Each subdirectory must supply rules for building sources it contributes
Src/JSON/%.o Src/JSON/%.su Src/JSON/%.cyclo: ../Src/JSON/%.c Src/JSON/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F302xC -c -I../Inc -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/chip_headers/CMSIS/Device/ST/STM32F3xx/Include" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/chip_headers/CMSIS/Include" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/A7672" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/ExtFlashMem" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/JSON" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/osKernel" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/System" -I"D:/GIT_WORKSPACE/VendingMachine/Vending_Machine_v2/Inc/Misc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-JSON

clean-Src-2f-JSON:
	-$(RM) ./Src/JSON/JSONParser.cyclo ./Src/JSON/JSONParser.d ./Src/JSON/JSONParser.o ./Src/JSON/JSONParser.su ./Src/JSON/JSONWriter.cyclo ./Src/JSON/JSONWriter.d ./Src/JSON/JSONWriter.o ./Src/JSON/JSONWriter.su

.PHONY: clean-Src-2f-JSON

