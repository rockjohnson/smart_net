################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/network/sn_net_addr.cpp \
../src/network/sn_socket_impl.cpp 

OBJS += \
./src/network/sn_net_addr.o \
./src/network/sn_socket_impl.o 

CPP_DEPS += \
./src/network/sn_net_addr.d \
./src/network/sn_socket_impl.d 


# Each subdirectory must supply rules for building sources it contributes
src/network/%.o: ../src/network/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/rock/Workspace/smart_net/smart_net/comm_utils/src" -I"/home/rock/Workspace/smart_net/smart_net/rmp" -O3 -Wall -c -fmessage-length=0 -Wreorder -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


