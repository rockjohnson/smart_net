################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../comm_utils/src/log/smart_log.cpp 

OBJS += \
./comm_utils/src/log/smart_log.o 

CPP_DEPS += \
./comm_utils/src/log/smart_log.d 


# Each subdirectory must supply rules for building sources it contributes
comm_utils/src/log/%.o: ../comm_utils/src/log/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


