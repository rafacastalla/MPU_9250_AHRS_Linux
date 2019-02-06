################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../libs/PIGPIO/command.c \
../libs/PIGPIO/pig2vcd.c \
../libs/PIGPIO/pigpio.c \
../libs/PIGPIO/pigpiod.c \
../libs/PIGPIO/pigpiod_if.c \
../libs/PIGPIO/pigpiod_if2.c \
../libs/PIGPIO/pigs.c \
../libs/PIGPIO/x_pigpio.c \
../libs/PIGPIO/x_pigpiod_if.c \
../libs/PIGPIO/x_pigpiod_if2.c 

O_SRCS += \
../libs/PIGPIO/command.o \
../libs/PIGPIO/pigpio.o 

OBJS += \
./libs/PIGPIO/command.o \
./libs/PIGPIO/pig2vcd.o \
./libs/PIGPIO/pigpio.o \
./libs/PIGPIO/pigpiod.o \
./libs/PIGPIO/pigpiod_if.o \
./libs/PIGPIO/pigpiod_if2.o \
./libs/PIGPIO/pigs.o \
./libs/PIGPIO/x_pigpio.o \
./libs/PIGPIO/x_pigpiod_if.o \
./libs/PIGPIO/x_pigpiod_if2.o 

C_DEPS += \
./libs/PIGPIO/command.d \
./libs/PIGPIO/pig2vcd.d \
./libs/PIGPIO/pigpio.d \
./libs/PIGPIO/pigpiod.d \
./libs/PIGPIO/pigpiod_if.d \
./libs/PIGPIO/pigpiod_if2.d \
./libs/PIGPIO/pigs.d \
./libs/PIGPIO/x_pigpio.d \
./libs/PIGPIO/x_pigpiod_if.d \
./libs/PIGPIO/x_pigpiod_if2.d 


# Each subdirectory must supply rules for building sources it contributes
libs/PIGPIO/%.o: ../libs/PIGPIO/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


