################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../smart_pbx/business/sp_dev_service.cpp \
../smart_pbx/business/sp_share.cpp 

OBJS += \
./smart_pbx/business/sp_dev_service.o \
./smart_pbx/business/sp_share.o 

CPP_DEPS += \
./smart_pbx/business/sp_dev_service.d \
./smart_pbx/business/sp_share.d 


# Each subdirectory must supply rules for building sources it contributes
smart_pbx/business/%.o: ../smart_pbx/business/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


