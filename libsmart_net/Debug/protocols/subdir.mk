################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../protocols/rmp_wrapper.cpp \
../protocols/rup_wrapper.cpp \
../protocols/tcp_wrapper_impl.cpp 

OBJS += \
./protocols/rmp_wrapper.o \
./protocols/rup_wrapper.o \
./protocols/tcp_wrapper_impl.o 

CPP_DEPS += \
./protocols/rmp_wrapper.d \
./protocols/rup_wrapper.d \
./protocols/tcp_wrapper_impl.d 


# Each subdirectory must supply rules for building sources it contributes
protocols/%.o: ../protocols/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I"/home/rock/Workspace/smart_net/smart_net/comm_utils" -O0 -g3 -Wall -c -fmessage-length=0 -fno-rtti -fno-exceptions -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


