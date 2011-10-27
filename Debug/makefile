################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include smart_pbx/tmp/subdir.mk
-include smart_pbx/src/subdir.mk
-include smart_pbx/business/subdir.mk
-include smart_net_test/src/subdir.mk
-include smart_net_test/busi/subdir.mk
-include smart_net_test/Release/src/subdir.mk
-include smart_net_test/Release/busi/subdir.mk
-include smart_net_test/Debug/src/subdir.mk
-include smart_net_test/Debug/busi/subdir.mk
-include smart_net/src/smart_net/subdir.mk
-include smart_net/src/network/subdir.mk
-include smart_net/src/framework/subdir.mk
-include smart_net/src/common/subdir.mk
-include smart_net/Release/src/smart_net/subdir.mk
-include smart_net/Release/src/network/subdir.mk
-include smart_net/Release/src/framework/subdir.mk
-include smart_net/Release/src/common/subdir.mk
-include smart_net/Debug/src/smart_net/subdir.mk
-include smart_net/Debug/src/network/subdir.mk
-include smart_net/Debug/src/framework/subdir.mk
-include smart_net/Debug/src/common/subdir.mk
-include rup/subdir.mk
-include comm_utils_test/src/subdir.mk
-include comm_utils_test/Debug/src/subdir.mk
-include comm_utils/src/utils/subdir.mk
-include comm_utils/src/thread/subdir.mk
-include comm_utils/src/memory/subdir.mk
-include comm_utils/src/log/subdir.mk
-include comm_utils/src/common/subdir.mk
-include comm_utils/Release/src/utils/subdir.mk
-include comm_utils/Release/src/thread/subdir.mk
-include comm_utils/Release/src/memory/subdir.mk
-include comm_utils/Release/src/log/subdir.mk
-include comm_utils/Release/src/common/subdir.mk
-include comm_utils/Debug/src/utils/subdir.mk
-include comm_utils/Debug/src/thread/subdir.mk
-include comm_utils/Debug/src/memory/subdir.mk
-include comm_utils/Debug/src/log/subdir.mk
-include comm_utils/Debug/src/common/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: librmp.a

# Tool invocations
librmp.a: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC Archiver'
	ar -r "librmp.a" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C++_DEPS)$(C_DEPS)$(CC_DEPS)$(ARCHIVES)$(CPP_DEPS)$(CXX_DEPS)$(C_UPPER_DEPS) librmp.a
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
