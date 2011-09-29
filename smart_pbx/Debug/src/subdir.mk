################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/smart_pbx.cpp 

OBJS += \
./src/smart_pbx.o 

CPP_DEPS += \
./src/smart_pbx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/rock/Workspace/smart_net/smart_net/smart_net" -I"/home/rock/Workspace/smart_net/smart_net/comm_utils" -O0 -g3 -Wall -c -fmessage-length=0 -fno-rtti -fno-exceptions -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


