################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../miniAHRS/FastMath.c \
../miniAHRS/Quaternion.c \
../miniAHRS/miniAHRS.c \
../miniAHRS/miniMatrix.c 

OBJS += \
./miniAHRS/FastMath.o \
./miniAHRS/Quaternion.o \
./miniAHRS/miniAHRS.o \
./miniAHRS/miniMatrix.o 

C_DEPS += \
./miniAHRS/FastMath.d \
./miniAHRS/Quaternion.d \
./miniAHRS/miniAHRS.d \
./miniAHRS/miniMatrix.d 


# Each subdirectory must supply rules for building sources it contributes
miniAHRS/%.o: ../miniAHRS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-linux-gnueabihf-gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


