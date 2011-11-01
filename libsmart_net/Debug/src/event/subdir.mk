################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/event/sn_io_evt_notifier_impl.cpp 

OBJS += \
./src/event/sn_io_evt_notifier_impl.o 

CPP_DEPS += \
./src/event/sn_io_evt_notifier_impl.d 


# Each subdirectory must supply rules for building sources it contributes
src/event/%.o: ../src/event/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/rock/Workspace/smart_net/smart_net/comm_utils/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


