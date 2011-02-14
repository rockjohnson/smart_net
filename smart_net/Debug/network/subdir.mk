################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../network/connecter.cpp \
../network/endpoint.cpp \
../network/listener.cpp \
../network/net_addr.cpp \
../network/socket.cpp 

OBJS += \
./network/connecter.o \
./network/endpoint.o \
./network/listener.o \
./network/net_addr.o \
./network/socket.o 

CPP_DEPS += \
./network/connecter.d \
./network/endpoint.d \
./network/listener.d \
./network/net_addr.d \
./network/socket.d 


# Each subdirectory must supply rules for building sources it contributes
network/%.o: ../network/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/rock/workspace/smartnet/comm_utils" -O0 -g3 -Wall -c -fmessage-length=0 -M -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


