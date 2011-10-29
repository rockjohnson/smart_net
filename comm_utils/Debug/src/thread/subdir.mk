################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/thread/thread_ex.cpp 

OBJS += \
./src/thread/thread_ex.o 

CPP_DEPS += \
./src/thread/thread_ex.d 


# Each subdirectory must supply rules for building sources it contributes
src/thread/%.o: ../src/thread/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


