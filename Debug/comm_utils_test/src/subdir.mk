################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../comm_utils_test/src/comm_utils_test.cpp 

OBJS += \
./comm_utils_test/src/comm_utils_test.o 

CPP_DEPS += \
./comm_utils_test/src/comm_utils_test.d 


# Each subdirectory must supply rules for building sources it contributes
comm_utils_test/src/%.o: ../comm_utils_test/src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


