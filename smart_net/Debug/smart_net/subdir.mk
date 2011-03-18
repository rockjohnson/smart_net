################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../smart_net/smart_net.cpp \
../smart_net/sn_endpoint_impl.cpp 

OBJS += \
./smart_net/smart_net.o \
./smart_net/sn_endpoint_impl.o 

CPP_DEPS += \
./smart_net/smart_net.d \
./smart_net/sn_endpoint_impl.d 


# Each subdirectory must supply rules for building sources it contributes
smart_net/%.o: ../smart_net/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/rock/workspace/smart_net/comm_utils" -O0 -g3 -Wall -c -fmessage-length=0 -fno-rtti -fno-exceptions -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


