################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../framework/sn_engine.cpp \
../framework/sn_io_evt_notify.cpp \
../framework/sn_io_obj.cpp \
../framework/sn_io_task.cpp 

OBJS += \
./framework/sn_engine.o \
./framework/sn_io_evt_notify.o \
./framework/sn_io_obj.o \
./framework/sn_io_task.o 

CPP_DEPS += \
./framework/sn_engine.d \
./framework/sn_io_evt_notify.d \
./framework/sn_io_obj.d \
./framework/sn_io_task.d 


# Each subdirectory must supply rules for building sources it contributes
framework/%.o: ../framework/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/rock/workspace/smartnet/comm_utils" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


