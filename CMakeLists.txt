cmake_minimum_required(VERSION 3.15)
project(BytecodeVM LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(DEBUGGER "Enable debug messages" OFF)

# Sources
add_executable(BytecodeVM
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

# Project includes (headers in root or include/)
target_include_directories(BytecodeVM PRIVATE
    ${CMAKE_SOURCE_DIR}
)

# pybind11 (embedded Python)
add_subdirectory(extern/pybind11)
target_link_libraries(BytecodeVM PRIVATE pybind11::embed)

# Debug macro wiring
if (DEBUGGER)
    target_compile_definitions(BytecodeVM PRIVATE DEBUGGER)
endif()

# Output directory (optional, but clean)
set_target_properties(BytecodeVM PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
