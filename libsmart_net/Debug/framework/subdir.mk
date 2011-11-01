################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../framework/sn_connecter.cpp \
../framework/sn_endpoint.cpp \
../framework/sn_mgr.cpp 

OBJS += \
./framework/sn_connecter.o \
./framework/sn_endpoint.o \
./framework/sn_mgr.o 

CPP_DEPS += \
./framework/sn_connecter.d \
./framework/sn_endpoint.d \
./framework/sn_mgr.d 


# Each subdirectory must supply rules for building sources it contributes
framework/%.o: ../framework/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/rock/Workspace/smart_net/smart_net/comm_utils" -O0 -g3 -Wall -c -fmessage-length=0 -fno-rtti -fno-exceptions -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


