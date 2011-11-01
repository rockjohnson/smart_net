################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../network/sn_connecter.cpp \
../network/sn_listener_impl.cpp \
../network/sn_net_addr.cpp \
../network/sn_socket_impl.cpp 

OBJS += \
./network/sn_connecter.o \
./network/sn_listener_impl.o \
./network/sn_net_addr.o \
./network/sn_socket_impl.o 

CPP_DEPS += \
./network/sn_connecter.d \
./network/sn_listener_impl.d \
./network/sn_net_addr.d \
./network/sn_socket_impl.d 


# Each subdirectory must supply rules for building sources it contributes
network/%.o: ../network/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/rock/Workspace/smart_net/smart_net/comm_utils" -O0 -g3 -Wall -c -fmessage-length=0 -fno-rtti -fno-exceptions -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


