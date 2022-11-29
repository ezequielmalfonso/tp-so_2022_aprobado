################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/comunicacion.c \
../src/main.c \
../src/memoriaConfig.c \
../src/memoria_utils.c \
../src/swap.c 

C_DEPS += \
./src/comunicacion.d \
./src/main.d \
./src/memoriaConfig.d \
./src/memoria_utils.d \
./src/swap.d 

OBJS += \
./src/comunicacion.o \
./src/main.o \
./src/memoriaConfig.o \
./src/memoria_utils.o \
./src/swap.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/Desktop/tp-2022-2c-Grupo-54/shared-lib/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/comunicacion.d ./src/comunicacion.o ./src/main.d ./src/main.o ./src/memoriaConfig.d ./src/memoriaConfig.o ./src/memoria_utils.d ./src/memoria_utils.o ./src/swap.d ./src/swap.o

.PHONY: clean-src

