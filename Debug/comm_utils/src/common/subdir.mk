################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../comm_utils/src/common/base.cpp \
../comm_utils/src/common/err_info.cpp 

OBJS += \
./comm_utils/src/common/base.o \
./comm_utils/src/common/err_info.o 

CPP_DEPS += \
./comm_utils/src/common/base.d \
./comm_utils/src/common/err_info.d 


# Each subdirectory must supply rules for building sources it contributes
comm_utils/src/common/%.o: ../comm_utils/src/common/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


