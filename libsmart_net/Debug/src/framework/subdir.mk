################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/framework/sn_engine.cpp \
../src/framework/sn_io_evt_notifier_impl.cpp \
../src/framework/sn_io_task.cpp \
../src/framework/sn_misc_task.cpp \
../src/framework/sn_time_notifier.cpp 

OBJS += \
./src/framework/sn_engine.o \
./src/framework/sn_io_evt_notifier_impl.o \
./src/framework/sn_io_task.o \
./src/framework/sn_misc_task.o \
./src/framework/sn_time_notifier.o 

CPP_DEPS += \
./src/framework/sn_engine.d \
./src/framework/sn_io_evt_notifier_impl.d \
./src/framework/sn_io_task.d \
./src/framework/sn_misc_task.d \
./src/framework/sn_time_notifier.d 


# Each subdirectory must supply rules for building sources it contributes
src/framework/%.o: ../src/framework/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/rock/Workspace/smart_net/smart_net/libcomm_utils/src" -I"/home/rock/Workspace/smart_net/smart_net/libcomm_net/src" -O0 -g3 -Wall -c -fmessage-length=0 -Wreorder -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


