################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../smart_net_test/src/main.cpp 

OBJS += \
./smart_net_test/src/main.o 

CPP_DEPS += \
./smart_net_test/src/main.d 


# Each subdirectory must supply rules for building sources it contributes
smart_net_test/src/%.o: ../smart_net_test/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


