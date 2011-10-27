################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../comm_utils/src/utils/event_engine.cpp \
../comm_utils/src/utils/file.cpp \
../comm_utils/src/utils/time_info.cpp \
../comm_utils/src/utils/utils.cpp 

OBJS += \
./comm_utils/src/utils/event_engine.o \
./comm_utils/src/utils/file.o \
./comm_utils/src/utils/time_info.o \
./comm_utils/src/utils/utils.o 

CPP_DEPS += \
./comm_utils/src/utils/event_engine.d \
./comm_utils/src/utils/file.d \
./comm_utils/src/utils/time_info.d \
./comm_utils/src/utils/utils.d 


# Each subdirectory must supply rules for building sources it contributes
comm_utils/src/utils/%.o: ../comm_utils/src/utils/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


