cmake_minimum_required(VERSION 2.8)
# Set policy: link_directories is relative to source dir
cmake_policy(SET CMP0015 NEW)

# Set the project name.
project(BIRCC C)

# tell CMake to search first in the cmake subdirectory for FIND_PACKAGE() or INCLUDE()
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/)

include(CheckCSourceCompiles)

if(CMAKE_COMPILER_IS_GNUCC)
 set(COMPILER_GCC 1)
set(CMAKE_C_FLAGS "-W -Wall -Wno-unused -Wno-unknown-pragmas -g -std=c89 -ffast-math -fstrict-aliasing -Wstrict-aliasing=2")
# set(CMAKE_LD_FLAGS "-pg")
# Use c89, debugging, all warnings except unused and unknown pragmas.
# when compiling with gnu compiler.  
# Warn about alisasing because otherwise aliasing problems it may not be detected.
endif(CMAKE_COMPILER_IS_GNUCC)

# Set include and lib dirs. 
include_directories("include" "include/chic")

set(BIRCC_SRC_FILES src/chic_inli.c src/memory.c src/pool.c src/bstrlib.c src/bstraux.c src/slre.c src/token.c src/cpp.c src/main.c)
message(STATUS ${BIRCC_SRC_FILES})
set_source_files_properties(${BIRCC_SRC_FILES} PROPERTIES LANGUAGE C)

# # exe in the bin subdir (first one should work, but doesnt, hmmm...)
# set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
add_executable(bircc ${BIRCC_SRC_FILES})

# enable_testing()
# Let ctest run valgrind
# test exe in the test subdir (first one should work, but doesnt, hmmm...)
# set(CMAKE_CTEST_OPTIONS --output-on-failure)
# set(CTEST_OUTPUT_ON_FAILURE TRUE)
# set(CTEST_MEMORYCHECK_COMMAND:FILEPATH "/usr/bin/valgrind")
# add_subdirectory("test")









