################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../busi/rmp_endpoint_tester.cpp \
../busi/tcp_endpoint_tester.cpp 

OBJS += \
./busi/rmp_endpoint_tester.o \
./busi/tcp_endpoint_tester.o 

CPP_DEPS += \
./busi/rmp_endpoint_tester.d \
./busi/tcp_endpoint_tester.d 


# Each subdirectory must supply rules for building sources it contributes
busi/%.o: ../busi/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/rock/Workspace/smart_net/smart_net/smart_net/src" -I"/home/rock/Workspace/smart_net/smart_net/rmp/src" -I"/home/rock/Workspace/smart_net/smart_net/comm_utils/src" -O3 -Wall -c -fmessage-length=0 -Wreorder -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


