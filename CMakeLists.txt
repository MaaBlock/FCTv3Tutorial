
cmake_minimum_required(VERSION 3.22.1)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

set(CMAKE_CXX_STANDARD 20)

message(STATUS "${CMAKE_TOOLCHAIN_FILE}")

if(ANDROID OR CMAKE_SYSTEM_NAME STREQUAL "Android" OR DEFINED CMAKE_ANDROID_API OR DEFINED ANDROID_PLATFORM)
    set(VCPKG_TARGET_ANDROID true)
    set(FCT_ANDROID true)
    message(STATUS "Building for Android")
else()
    set(VCPKG_TARGET_ANDROID false)
    set(FCT_ANDROID false)
    message(STATUS "Not building for Android")
endif()

if(WIN32)
    set(FCT_WIN32 true)
    message(STATUS "Building for Win32")
else()
    set(FCT_WIN32 false)
    message(STATUS "Not building for Android")
endif()

if (VCPKG_TARGET_ANDROID)
    include("vcpkg_android.cmake")
    set(VCPKG_CMAKE_SYSTEM_NAME Android)
    set(Android_INCLUDES ${Android_INCLUDES} ${CMAKE_SOURCE_DIR}/android/app/libs/include)
    set(Android_LIBRARIES ${Android_LIBRARIES} ${CMAKE_SOURCE_DIR}/android/app/libs/lib/${CMAKE_ANDROID_ARCH_ABI})

    include_directories(Android_INCLUDES)
endif()


if (FCT_ANDROID)
    project("androidtutorial")
    
    add_subdirectory(FCT)
    
    add_library(${CMAKE_PROJECT_NAME} SHARED
            main.cpp)
    
    target_link_libraries(${CMAKE_PROJECT_NAME}
            -uJava_com_example_androidtutorial_FCTMainActivity_CPPEntry
            FCT
            android
            log)
else()
    project("tutorial")
    
    add_subdirectory(FCT)
    
    add_executable(${CMAKE_PROJECT_NAME}
            main.cpp)
    
    target_link_libraries(${CMAKE_PROJECT_NAME}
            FCT)
endif()