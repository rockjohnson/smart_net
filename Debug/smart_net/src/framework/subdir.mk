################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../smart_net/src/framework/sn_engine.cpp \
../smart_net/src/framework/sn_io_evt_notifier_impl.cpp \
../smart_net/src/framework/sn_io_task.cpp \
../smart_net/src/framework/sn_misc_task.cpp \
../smart_net/src/framework/sn_time_notifier.cpp 

OBJS += \
./smart_net/src/framework/sn_engine.o \
./smart_net/src/framework/sn_io_evt_notifier_impl.o \
./smart_net/src/framework/sn_io_task.o \
./smart_net/src/framework/sn_misc_task.o \
./smart_net/src/framework/sn_time_notifier.o 

CPP_DEPS += \
./smart_net/src/framework/sn_engine.d \
./smart_net/src/framework/sn_io_evt_notifier_impl.d \
./smart_net/src/framework/sn_io_task.d \
./smart_net/src/framework/sn_misc_task.d \
./smart_net/src/framework/sn_time_notifier.d 


# Each subdirectory must supply rules for building sources it contributes
smart_net/src/framework/%.o: ../smart_net/src/framework/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


