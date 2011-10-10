################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/smart_net_test.cpp 

OBJS += \
./src/smart_net_test.o 

CPP_DEPS += \
./src/smart_net_test.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/rock/Workspace/smart_net/smart_net/smart_net" -I"/home/rock/Workspace/smart_net/smart_net/comm_utils/src" -O0 -g3 -Wall -c -fmessage-length=0 -M -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


