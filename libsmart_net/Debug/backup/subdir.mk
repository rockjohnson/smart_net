################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../backup/sn_service.cpp 

OBJS += \
./backup/sn_service.o 

CPP_DEPS += \
./backup/sn_service.d 


# Each subdirectory must supply rules for building sources it contributes
backup/%.o: ../backup/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/rock/Workspace/smart_net/smart_net/comm_utils" -O0 -g3 -Wall -c -fmessage-length=0 -fno-rtti -fno-exceptions -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


