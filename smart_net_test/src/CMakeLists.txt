# Make sure the compiler can find include files from our Hello library.
SET(CMAKE_CXX_FLAGS " -Wall")
include_directories (${smart_net_test_SOURCE_DIR}/../libcomm_utils/src)
include_directories (${smart_net_test_SOURCE_DIR}/../libsmart_net/src)
include_directories (${smart_net_test_SOURCE_DIR}/../libcomm_net/src)
#include_directories(/home/rock/boost_1_39_0)
#include_directories(/usr/include/python2.5)

# Make sure the linker can find the Hello library once it is built.
link_directories(${smart_net_test_SOURCE_DIR}/../libcomm_utils/build/lib)
link_directories(${smart_net_test_SOURCE_DIR}/../libsmart_net/build/lib)
link_directories(${smart_net_test_SOURCE_DIR}/../libcomm_net/build/lib)
#link_directories(/home/rock/Download/boost_1_39_0/bin.v2/libs/python/build/gcc-4.3.2/debug)

aux_source_directory(. SRC_FILES)
aux_source_directory(./busi/ SRC_FILES)

# Add executable called "helloDemo" that is built from the source files
# "demo.cxx" and "demo_b.cxx". The extensions are automatically found.
add_executable (smart_net_test ${SRC_FILES})

# Link the executable to the Hello library.
target_link_libraries (smart_net_test
smart_net
comm_net
comm_utils
pthread
tcmalloc
) 
