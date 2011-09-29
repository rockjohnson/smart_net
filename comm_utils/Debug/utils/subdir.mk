################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../utils/event_engine.cpp \
../utils/file.cpp \
../utils/time_info.cpp \
../utils/utils.cpp 

OBJS += \
./utils/event_engine.o \
./utils/file.o \
./utils/time_info.o \
./utils/utils.o 

CPP_DEPS += \
./utils/event_engine.d \
./utils/file.d \
./utils/time_info.d \
./utils/utils.d 


# Each subdirectory must supply rules for building sources it contributes
utils/%.o: ../utils/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -fno-rtti -fno-exceptions -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


