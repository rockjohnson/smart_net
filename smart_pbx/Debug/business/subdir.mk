################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../business/sp_dev_service.cpp \
../business/sp_share.cpp 

OBJS += \
./business/sp_dev_service.o \
./business/sp_share.o 

CPP_DEPS += \
./business/sp_dev_service.d \
./business/sp_share.d 


# Each subdirectory must supply rules for building sources it contributes
business/%.o: ../business/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/rock/workspace/smartnet/smart_net" -I"/home/rock/workspace/smartnet/comm_utils" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


