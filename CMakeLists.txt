cmake_minimum_required(VERSION 2.6)
project(libcr)

# Ensure all dependencies are existent.
execute_process(
	COMMAND git submodule init
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
execute_process(
	COMMAND git submodule update
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(depend/timer)

OPTION(LIBCR_RELEASE OFF "Whether to compile libcr in release mode")
OPTION(LIBCR_COMPACT_IP OFF "Whether to enable compact instruction pointers")
OPTION(LIBCR_INLINE OFF "Whether to inline libcr implementations")

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -Werror -g")

if(LIBCR_RELEASE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLIBCR_RELEASE=1 -O2 -mtune=native")
endif()

if(LIBCR_INLINE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLIBCR_INLINE=1")
endif()

if(COMPACT_IP)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLIBCR_COMPACT_IP")
endif()

# Select all source files.
file(GLOB_RECURSE libcr_sources ./src/*.cpp)
# Select all header files.
file(GLOB_RECURSE libcr_headers ./src/*.hpp ./src/*.inl)

# Add them to the project.
add_library(libcr STATIC ${libcr_sources} ${libcr_headers})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/libcr/ FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")
if(LIBCR_INLINE)
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/libcr/ FILES_MATCHING PATTERN "*.cpp")
endif()
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/libcr/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/depend/timer/include/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/)