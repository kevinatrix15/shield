set(this_target save-bb8.Test)

# Select all c++ files
file(GLOB CPP "*.cpp")
# Select all header files
file(GLOB H "*.h")

# Set binary for tests creation
add_executable(${this_target} ${CPP} ${H})

target_include_directories(
  ${this_target} PRIVATE "${CMAKE_SOURCE_DIR}/src"
)

source_group("Header Files" FILES ${H})
source_group("Source Files" FILES ${CPP})

# Set up Catch lib for testing
add_library(catch INTERFACE)
target_include_directories(catch INTERFACE
 "${CMAKE_SOURCE_DIR}/catch"
)

set(LIBS save-bb8 catch)

# Set libraries needed by the test binary
target_link_libraries(${this_target} ${LIBS})

# Set projects this test depends on
add_dependencies(${this_target} save-bb8)

# Set the test binary to run
add_test(NAME ${this_target} COMMAND ${this_target})
