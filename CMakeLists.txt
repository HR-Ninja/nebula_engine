cmake_minimum_required(VERSION 3.10)
project(NebulaEngine C)

set(CMAKE_C_STANDARD 11)
set(ASSETS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/assets)

# Include header files
include_directories(${CMAKE_SOURCE_DIR}/include)

# Link against your lib folder
link_directories(${CMAKE_SOURCE_DIR}/lib)

# Add source files
file(GLOB SOURCES "src/*.c")

# Create executable
add_executable(NebulaEngine ${SOURCES})

# Copy the assets directory to the build directory
add_custom_command(
    TARGET NebulaEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${ASSETS_SOURCE_DIR}
            $<TARGET_FILE_DIR:NebulaEngine>/assets
)

# Link libraries
target_link_libraries(NebulaEngine 
    PRIVATE
    glfw3
    opengl32
)
