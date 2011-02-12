################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../network/net_addr.cpp \
../network/net_connecter.cpp \
../network/net_listener.cpp \
../network/socket.cpp 

OBJS += \
./network/net_addr.o \
./network/net_connecter.o \
./network/net_listener.o \
./network/socket.o 

CPP_DEPS += \
./network/net_addr.d \
./network/net_connecter.d \
./network/net_listener.d \
./network/socket.d 


# Each subdirectory must supply rules for building sources it contributes
network/%.o: ../network/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/rock/workspace/smart_net/comm_utils" -O0 -g3 -Wall -c -fmessage-length=0 -M -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


