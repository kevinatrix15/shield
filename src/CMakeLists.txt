set(this_target save-bb8)

# Specify output folders
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# select all c++ files as CPP
file(GLOB CPP "*.cpp")
# Select all header files
file(GLOB H "*.h")

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX /Zi /MD)
    add_link_options(/Zi)
    #add_link_options(/W4 /WX)
    #add_compile_options(/W4 /WX /Zi /MD /D )
    #add_link_options(/W4 /WX /Zi /MD /D )
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-g -pg -ggdb -O2 -shared-libgcc -Wall -Wextra -pedantic -Werror)
    add_link_options(-g -pg -ggdb -O2 -shared-libgcc)
endif()

# setup library ###############################################################

# generate static library
add_library(${this_target} STATIC ${CPP} ${H})
source_group("Header Files" FILES ${H})
source_group("Source Files" FILES ${CPP})
set_target_properties(${this_target} PROPERTIES OUTPUT_NAME ${this_target})
# Specify output folders
set_target_properties(
  ${this_target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
                            LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

set(LIBS ${this_target})

# Set binary for tests creation
add_executable(${this_target}_bin "Main.cpp")
set_target_properties(${this_target}_bin PROPERTIES OUTPUT_NAME ${this_target})
target_link_libraries(${this_target}_bin ${LIBS})

# Handle tests
add_subdirectory(test)
