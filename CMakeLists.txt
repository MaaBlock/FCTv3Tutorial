# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.22.1)


if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()
set(VCPKG_TARGET_ANDROID TRUE)

if (VCPKG_TARGET_ANDROID)
    include("vcpkg_android.cmake")
    set(Android_INCLUDES ${Android_INCLUDES} ${CMAKE_SOURCE_DIR}/android/app/libs/include)
    set(Android_LIBRARIES ${Android_LIBRARIES} ${CMAKE_SOURCE_DIR}/android/app/libs/lib/${CMAKE_ANDROID_ARCH_ABI})
    include_directories(Android_INCLUDES)
endif()

project("tutorial")

# Creates your game shared library. The name must be the same as the
# one used for loading in your Kotlin/Java or AndroidManifest.txt files.
add_library(${PROJECT_NAME} SHARED
        main.cpp
        FCT/Runtime/AndroidOut.cpp
        Renderer.cpp
        Shader.cpp
        TextureAsset.cpp
        Utility.cpp)

# Searches for a package provided by the game activity dependency
#find_package(game-activity REQUIRED CONFIG)


add_subdirectory ("FCT")

# Configure libraries CMake uses to link your target library.
target_link_libraries(${PROJECT_NAME}
        -uJava_com_google_androidgamesdk_GameActivity_initializeNativeCode
        FCT

        # The game activity
#        game-activity::game-activity

        # EGL and other dependent libraries required for drawing
        # and interacting with Android system
        EGL
        GLESv3
        jnigraphics
        android
        log)