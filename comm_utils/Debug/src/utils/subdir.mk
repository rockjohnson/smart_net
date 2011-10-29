################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/utils/event_engine.cpp \
../src/utils/file.cpp \
../src/utils/time_info.cpp \
../src/utils/utils.cpp 

OBJS += \
./src/utils/event_engine.o \
./src/utils/file.o \
./src/utils/time_info.o \
./src/utils/utils.o 

CPP_DEPS += \
./src/utils/event_engine.d \
./src/utils/file.d \
./src/utils/time_info.d \
./src/utils/utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/utils/%.o: ../src/utils/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


