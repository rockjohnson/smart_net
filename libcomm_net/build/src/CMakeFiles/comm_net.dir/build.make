# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Produce verbose output by default.
VERBOSE = 1

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/rock/Workspace/smart_net/smart_net/libcomm_net

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/rock/Workspace/smart_net/smart_net/libcomm_net/build

# Include any dependencies generated for this target.
include src/CMakeFiles/comm_net.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/comm_net.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/comm_net.dir/flags.make

src/CMakeFiles/comm_net.dir/network/sn_net_addr.cpp.o: src/CMakeFiles/comm_net.dir/flags.make
src/CMakeFiles/comm_net.dir/network/sn_net_addr.cpp.o: ../src/network/sn_net_addr.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/rock/Workspace/smart_net/smart_net/libcomm_net/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/comm_net.dir/network/sn_net_addr.cpp.o"
	cd /home/rock/Workspace/smart_net/smart_net/libcomm_net/build/src && /usr/lib64/ccache/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/comm_net.dir/network/sn_net_addr.cpp.o -c /home/rock/Workspace/smart_net/smart_net/libcomm_net/src/network/sn_net_addr.cpp

src/CMakeFiles/comm_net.dir/network/sn_net_addr.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/comm_net.dir/network/sn_net_addr.cpp.i"
	cd /home/rock/Workspace/smart_net/smart_net/libcomm_net/build/src && /usr/lib64/ccache/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/rock/Workspace/smart_net/smart_net/libcomm_net/src/network/sn_net_addr.cpp > CMakeFiles/comm_net.dir/network/sn_net_addr.cpp.i

src/CMakeFiles/comm_net.dir/network/sn_net_addr.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/comm_net.dir/network/sn_net_addr.cpp.s"
	cd /home/rock/Workspace/smart_net/smart_net/libcomm_net/build/src && /usr/lib64/ccache/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/rock/Workspace/smart_net/smart_net/libcomm_net/src/network/sn_net_addr.cpp -o CMakeFiles/comm_net.dir/network/sn_net_addr.cpp.s

src/CMakeFiles/comm_net.dir/network/sn_net_addr.cpp.o.requires:
.PHONY : src/CMakeFiles/comm_net.dir/network/sn_net_addr.cpp.o.requires

src/CMakeFiles/comm_net.dir/network/sn_net_addr.cpp.o.provides: src/CMakeFiles/comm_net.dir/network/sn_net_addr.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/comm_net.dir/build.make src/CMakeFiles/comm_net.dir/network/sn_net_addr.cpp.o.provides.build
.PHONY : src/CMakeFiles/comm_net.dir/network/sn_net_addr.cpp.o.provides

src/CMakeFiles/comm_net.dir/network/sn_net_addr.cpp.o.provides.build: src/CMakeFiles/comm_net.dir/network/sn_net_addr.cpp.o

src/CMakeFiles/comm_net.dir/network/sn_socket_impl.cpp.o: src/CMakeFiles/comm_net.dir/flags.make
src/CMakeFiles/comm_net.dir/network/sn_socket_impl.cpp.o: ../src/network/sn_socket_impl.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/rock/Workspace/smart_net/smart_net/libcomm_net/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/comm_net.dir/network/sn_socket_impl.cpp.o"
	cd /home/rock/Workspace/smart_net/smart_net/libcomm_net/build/src && /usr/lib64/ccache/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/comm_net.dir/network/sn_socket_impl.cpp.o -c /home/rock/Workspace/smart_net/smart_net/libcomm_net/src/network/sn_socket_impl.cpp

src/CMakeFiles/comm_net.dir/network/sn_socket_impl.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/comm_net.dir/network/sn_socket_impl.cpp.i"
	cd /home/rock/Workspace/smart_net/smart_net/libcomm_net/build/src && /usr/lib64/ccache/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/rock/Workspace/smart_net/smart_net/libcomm_net/src/network/sn_socket_impl.cpp > CMakeFiles/comm_net.dir/network/sn_socket_impl.cpp.i

src/CMakeFiles/comm_net.dir/network/sn_socket_impl.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/comm_net.dir/network/sn_socket_impl.cpp.s"
	cd /home/rock/Workspace/smart_net/smart_net/libcomm_net/build/src && /usr/lib64/ccache/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/rock/Workspace/smart_net/smart_net/libcomm_net/src/network/sn_socket_impl.cpp -o CMakeFiles/comm_net.dir/network/sn_socket_impl.cpp.s

src/CMakeFiles/comm_net.dir/network/sn_socket_impl.cpp.o.requires:
.PHONY : src/CMakeFiles/comm_net.dir/network/sn_socket_impl.cpp.o.requires

src/CMakeFiles/comm_net.dir/network/sn_socket_impl.cpp.o.provides: src/CMakeFiles/comm_net.dir/network/sn_socket_impl.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/comm_net.dir/build.make src/CMakeFiles/comm_net.dir/network/sn_socket_impl.cpp.o.provides.build
.PHONY : src/CMakeFiles/comm_net.dir/network/sn_socket_impl.cpp.o.provides

src/CMakeFiles/comm_net.dir/network/sn_socket_impl.cpp.o.provides.build: src/CMakeFiles/comm_net.dir/network/sn_socket_impl.cpp.o

# Object files for target comm_net
comm_net_OBJECTS = \
"CMakeFiles/comm_net.dir/network/sn_net_addr.cpp.o" \
"CMakeFiles/comm_net.dir/network/sn_socket_impl.cpp.o"

# External object files for target comm_net
comm_net_EXTERNAL_OBJECTS =

lib/libcomm_net.a: src/CMakeFiles/comm_net.dir/network/sn_net_addr.cpp.o
lib/libcomm_net.a: src/CMakeFiles/comm_net.dir/network/sn_socket_impl.cpp.o
lib/libcomm_net.a: src/CMakeFiles/comm_net.dir/build.make
lib/libcomm_net.a: src/CMakeFiles/comm_net.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX static library ../lib/libcomm_net.a"
	cd /home/rock/Workspace/smart_net/smart_net/libcomm_net/build/src && $(CMAKE_COMMAND) -P CMakeFiles/comm_net.dir/cmake_clean_target.cmake
	cd /home/rock/Workspace/smart_net/smart_net/libcomm_net/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/comm_net.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/comm_net.dir/build: lib/libcomm_net.a
.PHONY : src/CMakeFiles/comm_net.dir/build

src/CMakeFiles/comm_net.dir/requires: src/CMakeFiles/comm_net.dir/network/sn_net_addr.cpp.o.requires
src/CMakeFiles/comm_net.dir/requires: src/CMakeFiles/comm_net.dir/network/sn_socket_impl.cpp.o.requires
.PHONY : src/CMakeFiles/comm_net.dir/requires

src/CMakeFiles/comm_net.dir/clean:
	cd /home/rock/Workspace/smart_net/smart_net/libcomm_net/build/src && $(CMAKE_COMMAND) -P CMakeFiles/comm_net.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/comm_net.dir/clean

src/CMakeFiles/comm_net.dir/depend:
	cd /home/rock/Workspace/smart_net/smart_net/libcomm_net/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rock/Workspace/smart_net/smart_net/libcomm_net /home/rock/Workspace/smart_net/smart_net/libcomm_net/src /home/rock/Workspace/smart_net/smart_net/libcomm_net/build /home/rock/Workspace/smart_net/smart_net/libcomm_net/build/src /home/rock/Workspace/smart_net/smart_net/libcomm_net/build/src/CMakeFiles/comm_net.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/comm_net.dir/depend

