################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/common/sn_err_info.cpp 

OBJS += \
./src/common/sn_err_info.o 

CPP_DEPS += \
./src/common/sn_err_info.d 


# Each subdirectory must supply rules for building sources it contributes
src/common/%.o: ../src/common/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/rock/Workspace/smart_net/smart_net/libcomm_utils/src" -I"/home/rock/Workspace/smart_net/smart_net/libcomm_net/src" -O0 -g3 -Wall -c -fmessage-length=0 -Wreorder -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


