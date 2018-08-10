cmake_minimum_required(VERSION 2.6)
project(libcr)

# Select all source files.
file(GLOB_RECURSE libcr_sources ./src/*.cpp)
# Select all header files.
file(GLOB_RECURSE libcr_headers ./src/*.hpp ./src/*.inl)

# Add them to the project.
add_library(libcr ${libcr_sources} ${libcr_headers})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include/libcr/ FILES_MATCHING PATTERN "*.hpp" PATTERN "*.inl")
