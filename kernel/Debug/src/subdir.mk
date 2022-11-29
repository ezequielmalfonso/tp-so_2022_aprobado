################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/comunicacion.c \
../src/kernelConfig.c \
../src/main.c \
../src/planificacion.c 

C_DEPS += \
./src/comunicacion.d \
./src/kernelConfig.d \
./src/main.d \
./src/planificacion.d 

OBJS += \
./src/comunicacion.o \
./src/kernelConfig.o \
./src/main.o \
./src/planificacion.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/Desktop/tp-2022-2c-Grupo-54/shared-lib/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/comunicacion.d ./src/comunicacion.o ./src/kernelConfig.d ./src/kernelConfig.o ./src/main.d ./src/main.o ./src/planificacion.d ./src/planificacion.o

.PHONY: clean-src

