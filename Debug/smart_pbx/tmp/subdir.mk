################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../smart_pbx/tmp/rmp_wrapper.cpp \
../smart_pbx/tmp/rup_wrapper.cpp \
../smart_pbx/tmp/smart_net.cpp \
../smart_pbx/tmp/sn_connecter.cpp \
../smart_pbx/tmp/sn_engine.cpp \
../smart_pbx/tmp/sn_listener_impl.cpp \
../smart_pbx/tmp/tcp_wrapper_impl.cpp 

OBJS += \
./smart_pbx/tmp/rmp_wrapper.o \
./smart_pbx/tmp/rup_wrapper.o \
./smart_pbx/tmp/smart_net.o \
./smart_pbx/tmp/sn_connecter.o \
./smart_pbx/tmp/sn_engine.o \
./smart_pbx/tmp/sn_listener_impl.o \
./smart_pbx/tmp/tcp_wrapper_impl.o 

CPP_DEPS += \
./smart_pbx/tmp/rmp_wrapper.d \
./smart_pbx/tmp/rup_wrapper.d \
./smart_pbx/tmp/smart_net.d \
./smart_pbx/tmp/sn_connecter.d \
./smart_pbx/tmp/sn_engine.d \
./smart_pbx/tmp/sn_listener_impl.d \
./smart_pbx/tmp/tcp_wrapper_impl.d 


# Each subdirectory must supply rules for building sources it contributes
smart_pbx/tmp/%.o: ../smart_pbx/tmp/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


