#----------------------- PROJECT CONFIGURATION --------------------------------
cmake_minimum_required(VERSION 3.16)
project(cpp-dotenv VERSION 1.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE RELEASE)
else()
    string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)
endif()
message(STATUS "Building CPP-DOTENV in ${CMAKE_BUILD_TYPE} mode")

# Option to build both static and shared libraries
option(BUILD_BOTH_LIBRARIES "Build both static and shared libraries" OFF)

# Validate that BUILD_BOTH_LIBRARIES and BUILD_SHARED_LIBS are not used together
if(BUILD_BOTH_LIBRARIES AND BUILD_SHARED_LIBS)
    message(WARNING "BUILD_BOTH_LIBRARIES and BUILD_SHARED_LIBS should not be used together. BUILD_SHARED_LIBS will be ignored.")
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
endif()

# When building shared libraries that link against static libraries,
# we need position-independent code for the static libraries
if(BUILD_SHARED_LIBS OR BUILD_BOTH_LIBRARIES)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

#------------------- SUBDIRECTORY ADDITION ------------------------------------

add_subdirectory(common)
add_subdirectory(src)

#----------------------- LIBRARY CONFIGURATION --------------------------------

set(CPP_DOTENV_SRC
    src/dotenv.cpp
    include/dotenv.h
)

# Common include directories
set(CPP_DOTENV_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Common compile options - handle MSVC vs GCC/Clang
if (MSVC)
    # MSVC-specific flags
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
        set(CPP_DOTENV_COMPILE_OPTIONS /W4 /Od)
    else()
        set(CPP_DOTENV_COMPILE_OPTIONS /W4 /O2)
    endif()
else()
    # GCC/Clang and other compilers using Unix-style flags
    if ("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
        set(CPP_DOTENV_COMPILE_OPTIONS -g -Wall -O0)
    else()
        set(CPP_DOTENV_COMPILE_OPTIONS -O3)
    endif()
endif()

# Build both static and shared libraries if requested
if(BUILD_BOTH_LIBRARIES)
    # Static library
    add_library(cpp_dotenv_static STATIC ${CPP_DOTENV_SRC})
    target_link_libraries(cpp_dotenv_static
        ${ENVIRON_LIB}
        ${PARSER_LIB}
    )
    target_include_directories(cpp_dotenv_static PUBLIC ${CPP_DOTENV_INCLUDE_DIRS})
    target_compile_options(cpp_dotenv_static PRIVATE ${CPP_DOTENV_COMPILE_OPTIONS})
    # Set output name to libcpp_dotenv.a (instead of libcpp_dotenv_static.a)
    set_target_properties(cpp_dotenv_static PROPERTIES OUTPUT_NAME cpp_dotenv)

    # Shared library
    add_library(cpp_dotenv_shared SHARED ${CPP_DOTENV_SRC})
    target_link_libraries(cpp_dotenv_shared
        ${ENVIRON_LIB}
        ${PARSER_LIB}
    )
    target_include_directories(cpp_dotenv_shared PUBLIC ${CPP_DOTENV_INCLUDE_DIRS})
    target_compile_options(cpp_dotenv_shared PRIVATE ${CPP_DOTENV_COMPILE_OPTIONS})
    # Set output name. On non-MSVC platforms, both static and shared libraries use
    # the same base name "cpp_dotenv". On MSVC, give the shared library a distinct
    # base name to avoid conflicts between the static library (.lib) and the
    # import library for the shared library (also .lib).
    if(MSVC)
        set_target_properties(cpp_dotenv_shared PROPERTIES OUTPUT_NAME cpp_dotenv_shared)
    else()
        set_target_properties(cpp_dotenv_shared PROPERTIES OUTPUT_NAME cpp_dotenv)
    endif()

    # Alias for backward compatibility:
    # When BUILD_BOTH_LIBRARIES is ON, the cpp_dotenv target is an alias to
    # cpp_dotenv_static. Projects that need the shared library must explicitly
    # link against cpp_dotenv_shared instead of cpp_dotenv.
    add_library(cpp_dotenv ALIAS cpp_dotenv_static)
else()
    # Single library (type determined by BUILD_SHARED_LIBS)
    set(CPP_DOTENV cpp_dotenv CACHE INTERNAL "")
    add_library(${CPP_DOTENV} ${CPP_DOTENV_SRC})

    target_link_libraries(${CPP_DOTENV}
        ${ENVIRON_LIB}
        ${PARSER_LIB}
    )

    target_include_directories(${CPP_DOTENV} PUBLIC
        ${CPP_DOTENV_INCLUDE_DIRS}
    )

    target_compile_options(${CPP_DOTENV} PRIVATE
        ${CPP_DOTENV_COMPILE_OPTIONS}
    )
endif()
