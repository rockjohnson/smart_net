################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../framework/io_evt_notify.cpp \
../framework/io_obj.cpp \
../framework/io_task.cpp \
../framework/net_engine.cpp 

OBJS += \
./framework/io_evt_notify.o \
./framework/io_obj.o \
./framework/io_task.o \
./framework/net_engine.o 

CPP_DEPS += \
./framework/io_evt_notify.d \
./framework/io_obj.d \
./framework/io_task.d \
./framework/net_engine.d 


# Each subdirectory must supply rules for building sources it contributes
framework/%.o: ../framework/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/rock/workspace/smart_net/comm_utils" -O0 -g3 -Wall -c -fmessage-length=0 -M -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


