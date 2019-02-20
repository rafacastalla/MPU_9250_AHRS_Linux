################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../AK8963.cpp \
../I2C.cpp \
../ImuRaw.cpp \
../MPU9250.cpp \
../MainAngles.cpp 

OBJS += \
./AK8963.o \
./I2C.o \
./ImuRaw.o \
./MPU9250.o \
./MainAngles.o 

CPP_DEPS += \
./AK8963.d \
./I2C.d \
./ImuRaw.d \
./MPU9250.d \
./MainAngles.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -std=c++0x -I"/home/racarla/eclipse-workspace/MPU_9250_AHRS_Linux/IMUalternative/includes" -I"/home/racarla/eclipse-workspace/MPU_9250_AHRS_Linux/IMUalternative/interfaces" -I"/home/racarla/eclipse-workspace/MPU_9250_AHRS_Linux/miniAHRS" -I"/home/racarla/eclipse-workspace/MPU_9250_AHRS_Linux/libs/Eigen" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


