cmake_minimum_required(VERSION 3.10)

project(
    OpcuaCs
    LANGUAGES C CXX
)

# ----------------------------------------------------------------------------
# Asmpro compile configure
# ----------------------------------------------------------------------------
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build)
set(CMAKE_INSTALL_PREFIX "/opt/asmpro")
set(Asmpro_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}")
set(Asmpro_CONFIG_INSTALL_PATH "lib/cmake/asmpro")
set(Asmpro_INSTALL_LIB_DIR "${Asmpro_INSTALL_PATH}/lib")
set(Asmpro_INSTALL_INCLUDE_DIR "${Asmpro_INSTALL_PATH}/include/asmpro")

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# ----------------------------------------------------------------------------
# Asmpro cmake options
# ----------------------------------------------------------------------------
include(cmake/AsmproModule.cmake)

option(BUILD_SHARED_LIBS "build shared libraries" OFF)

# build test option
option(BUILD_TESTS "Build accuracy & regression tests" OFF)

if(BUILD_TESTS)
    enable_testing()
    find_package(GTest)
    if(NOT GTest_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.12.1
        )
        # For Windows: Prevent overriding the parent project's compiler/linker settings
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif(NOT GTest_FOUND)
endif(BUILD_TESTS)

# build with sanitizers option
option(WITH_SANITIZERS "Build with a memory profiling tool: Santizers")

if(WITH_SANITIZERS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif(WITH_SANITIZERS)

# build Asmpro documents
option(BUILD_DOCS "Create build rules for Asmpro Documentation" OFF)

add_subdirectory(modules)

add_executable(server src/ua_server.cpp)
add_executable(client src/ua_client.cpp)

target_link_libraries(server asmpro_opcua_cs)
target_link_libraries(client asmpro_opcua_cs)
