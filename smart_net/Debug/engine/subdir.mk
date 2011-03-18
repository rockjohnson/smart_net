################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../engine/sn_engine.cpp \
../engine/sn_io_evt_notifier_impl.cpp \
../engine/sn_io_task.cpp \
../engine/sn_misc_task.cpp \
../engine/sn_time_notifier.cpp 

OBJS += \
./engine/sn_engine.o \
./engine/sn_io_evt_notifier_impl.o \
./engine/sn_io_task.o \
./engine/sn_misc_task.o \
./engine/sn_time_notifier.o 

CPP_DEPS += \
./engine/sn_engine.d \
./engine/sn_io_evt_notifier_impl.d \
./engine/sn_io_task.d \
./engine/sn_misc_task.d \
./engine/sn_time_notifier.d 


# Each subdirectory must supply rules for building sources it contributes
engine/%.o: ../engine/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/rock/workspace/smart_net/comm_utils" -O0 -g3 -Wall -c -fmessage-length=0 -fno-rtti -fno-exceptions -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


