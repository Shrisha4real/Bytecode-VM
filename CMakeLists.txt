cmake_minimum_required(VERSION 3.15)
project(BytecodeVM LANGUAGES CXX)

# Require C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
        
# Add all source files
set(SOURCES
    main.cpp
    Object.cpp
    ParseRule.cpp
    Parser.cpp
    Scanner.cpp
    StringInterner.cpp
    Table.cpp
    Token.cpp
    Value.cpp
    chunk.cpp
    compiler.cpp
    debug.cpp
    vm.cpp
)
# Include headers

set(CMAKE_BUILD_TYPE Debug)
include_directories(${CMAKE_SOURCE_DIR})

# Add pybind11 from extern/
add_subdirectory(extern/pybind11)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
# Define the executable
add_executable(BytecodeVM ${SOURCES})

# Link pybind11 (if you want to embed Python or expose bindings)
target_link_libraries(BytecodeVM PRIVATE pybind11::embed)

# Optionally: set output dir
set_target_properties(BytecodeVM PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
add_custom_target(build_and_run
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} 
    COMMAND ${CMAKE_BINARY_DIR}/bin/BytecodeVM ${CMAKE_SOURCE_DIR}/file.txt
    DEPENDS BytecodeVM
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
