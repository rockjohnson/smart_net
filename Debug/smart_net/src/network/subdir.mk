################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../smart_net/src/network/sn_net_addr.cpp \
../smart_net/src/network/sn_socket_impl.cpp 

OBJS += \
./smart_net/src/network/sn_net_addr.o \
./smart_net/src/network/sn_socket_impl.o 

CPP_DEPS += \
./smart_net/src/network/sn_net_addr.d \
./smart_net/src/network/sn_socket_impl.d 


# Each subdirectory must supply rules for building sources it contributes
smart_net/src/network/%.o: ../smart_net/src/network/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


