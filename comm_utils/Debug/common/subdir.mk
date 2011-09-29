################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../common/base.cpp \
../common/err_info.cpp 

OBJS += \
./common/base.o \
./common/err_info.o 

CPP_DEPS += \
./common/base.d \
./common/err_info.d 


# Each subdirectory must supply rules for building sources it contributes
common/%.o: ../common/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -fno-rtti -fno-exceptions -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


