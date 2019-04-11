cmake_minimum_required(VERSION 2.6)
project(libcr)

OPTION(LIBCR_RELEASE OFF)

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -Werror -g")

if(LIBCR_RELEASE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLIBCR_RELEASE=1 -O3 -mtune=native")
endif()

# Select all source files.
file(GLOB_RECURSE libcr_sources ./src/*.cpp)
# Select all header files.
file(GLOB_RECURSE libcr_headers ./src/*.hpp ./src/*.inl)

# Add them to the project.
add_library(libcr STATIC ${libcr_sources} ${libcr_headers})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/libcr/ FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/libcr/)
