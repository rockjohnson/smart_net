################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/rmp_wrapper.cpp \
../src/rup_wrapper.cpp \
../src/smart_net.cpp \
../src/sn_engine.cpp \
../src/tcp_wrapper_impl.cpp 

OBJS += \
./src/rmp_wrapper.o \
./src/rup_wrapper.o \
./src/smart_net.o \
./src/sn_engine.o \
./src/tcp_wrapper_impl.o 

CPP_DEPS += \
./src/rmp_wrapper.d \
./src/rup_wrapper.d \
./src/smart_net.d \
./src/sn_engine.d \
./src/tcp_wrapper_impl.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/rock/Workspace/smart_net/smart_net/smart_net/smart_net/comm_utils -O0 -g3 -Wall -c -fmessage-length=0 -fno-rtti -fno-exceptions -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


