################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/smart_net/sn_endpoint_impl.cpp 

OBJS += \
./src/smart_net/sn_endpoint_impl.o 

CPP_DEPS += \
./src/smart_net/sn_endpoint_impl.d 


# Each subdirectory must supply rules for building sources it contributes
src/smart_net/%.o: ../src/smart_net/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/rock/Workspace/smart_net/smart_net/libcomm_utils/src" -I"/home/rock/Workspace/smart_net/smart_net/libcomm_net/src" -O0 -g3 -Wall -c -fmessage-length=0 -Wreorder -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


