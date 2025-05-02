cmake_minimum_required(VERSION 3.10)
project(NebulaEngine C)

set(CMAKE_C_STANDARD 11)

# Include header files
include_directories(${CMAKE_SOURCE_DIR}/include)

# Link against your lib folder
link_directories(${CMAKE_SOURCE_DIR}/lib)

# Add source files
file(GLOB SOURCES "src/*.c")

# Create executable
add_executable(NebulaEngine ${SOURCES})

# Link libraries
target_link_libraries(NebulaEngine 
    PRIVATE
    glfw3
    opengl32
)
