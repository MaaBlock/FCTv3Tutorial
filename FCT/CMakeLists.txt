

file(GLOB RUNTIME_MODULE_SOURCES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./Runtime/*.cpp"
)

file(GLOB RUNTIME_MODULE_HEADERS
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./Runtime/*.h"
)

file(GLOB RUNTIME_ANDROID_MODULE_SOURCES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./Runtime/Android_*.cpp"
)

file(GLOB IMAGELOADER_FREETYPE_MODULE_SOURCES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./ImageLoader/FreeImage_*.cpp"
)

list(REMOVE_ITEM RUNTIME_MODULE_SOURCES ${RUNTIME_ANDROID_MODULE_SOURCES})

file(GLOB RUNTIME_WIN32_MODULE_SOURCES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./Runtime/Win32_*.cpp"
)
list(REMOVE_ITEM RUNTIME_MODULE_SOURCES ${RUNTIME_WIN32_MODULE_SOURCES})


file(GLOB RUNTIME_ANDROID_MODULE_HEADERS
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./Runtime/Android_*.h"
)
list(REMOVE_ITEM RUNTIME_MODULE_HEADERS ${RUNTIME_ANDROID_MODULE_HEADERS})

file(GLOB CONTEXT_MODULE_SOURCES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./Context/*.cpp"
)
file(GLOB CONTEXT_MODULE_HEADERS
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./Context/*.h"
)

file(GLOB RHI_MODULE_SOURCES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./RHI/*.cpp"
)
file(GLOB RHI_MODULE_HEADERS
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./RHI/*.h"
)
file(GLOB RHI_VULKAN_MODULE_SOURCES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./RHI/VK_*.cpp"
)
file(GLOB RHI_VULKAN_MODULE_HEADERS
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./RHI/VK_*.h"
)
list(REMOVE_ITEM RHI_MODULE_SOURCES ${RHI_VULKAN_MODULE_SOURCES})
list(REMOVE_ITEM RHI_MODULE_HEADERS ${RHI_VULKAN_MODULE_HEADERS})
file(GLOB CONTEXT_VULKAN_MODULE_SOURCES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./Context/VK_*.cpp"
)
file(GLOB CONTEXT_VULKAN_MODULE_HEADERS
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./Context/VK_*.h"
)
file(GLOB CONTEXT_SHARDEC_MODULE_SOURCES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./Context/Shaderc_*.cpp"
)

list(REMOVE_ITEM CONTEXT_MODULE_SOURCES ${CONTEXT_SHARDEC_MODULE_SOURCES})
file(GLOB DEBUGTOOLS_MODULE_HEADERS
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./DebugTools/[a-zA-Z0-9]*.h"
)
file(GLOB DEBUGTOOLS_MODULE_SOURCES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./DebugTools/*.cpp"
)
file(GLOB DEBUGTOOLS_ANDROID_MODULE_SOURCES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./DebugTools/Android_*.cpp"
)
list(REMOVE_ITEM DEBUGTOOLS_MODULE_SOURCES ${DEBUGTOOLS_ANDROID_MODULE_SOURCES})


file(GLOB UI_MODULE_SOURCES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./UI/*.cpp"
)
file(GLOB UI_MODULE_HEADERS
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./UI/*.h"
)

file(GLOB UI_GLFW_MODULE_SOURCES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./UI/GLFW_*.cpp"
)
file(GLOB UI_GLFW_MODULE_HEADERS
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./UI/GLFW_*.h"
)

list(REMOVE_ITEM UI_MODULE_SOURCES ${UI_GLFW_MODULE_SOURCES})

file(GLOB UI_ANDROID_MODULE_SOURCES
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./UI/Android_*.cpp"
)

list(REMOVE_ITEM UI_MODULE_SOURCES ${UI_ANDROID_MODULE_SOURCES})
file(GLOB UI_ANDROID_MODULE_HEADERS
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./UI/Android_*.h"
)
list(REMOVE_ITEM UI_MODULE_HEADERS ${UI_ANDROID_MODULE_HEADERS})

message(STATUS "DEBUGTOOLS_MODULE_SOURCES: ${DEBUGTOOLS_MODULE_SOURCES}")
message(STATUS "UI_MODULE_HEADERS: ${UI_MODULE_HEADERS}")
message(STATUS "RUNTIME_WIN32_MODULE_SOURCES: ${RUNTIME_WIN32_MODULE_SOURCES}")
#find_package(Boost REQUIRED COMPONENTS system thread locale)
#find_package(harfbuzz CONFIG REQUIRED)
#find_package(Freetype REQUIRED)
#find_package(Bullet CONFIG REQUIRED)
if (FCT_ANDROID)
    set(CMAKE_CXX_STANDARD 20)
    find_package(VulkanHeaders CONFIG REQUIRED)
else()
    set(CMAKE_CXX_STANDARD 17)
endif ()

add_library(FCT STATIC
    ${RUNTIME_MODULE_SOURCES}
    ${DEBUGTOOLS_MODULE_SOURCES}
    ${UI_MODULE_SOURCES}
    ${CONTEXT_MODULE_SOURCES}
    ${CONTEXT_VULKAN_MODULE_SOURCES}
    ${RHI_VULKAN_MODULE_SOURCES}
    ${RHI_MODULE_SOURCES}
    headers.cpp
)
target_sources(FCT PRIVATE
        ${CONTEXT_SHARDEC_MODULE_SOURCES}
)

if (FCT_WIN32)
    target_sources(FCT PRIVATE
        ${RUNTIME_WIN32_MODULE_SOURCES}
    )
endif()

if (FCT_ANDROID) 
    target_sources(FCT PRIVATE
        ${UI_ANDROID_MODULE_SOURCES}
        ${DEBUGTOOLS_ANDROID_MODULE_SOURCES}
    )
    target_link_libraries(FCT PUBLIC Vulkan::Headers)

else()
    target_sources(FCT PRIVATE
            ${UI_GLFW_MODULE_SOURCES}
            ${IMAGELOADER_FREETYPE_MODULE_SOURCES}
    )
    find_package(freeimage CONFIG REQUIRED)
    target_link_libraries(FCT PRIVATE freeimage::FreeImage freeimage::FreeImagePlus)

    find_package(glfw3 CONFIG REQUIRED)
    target_link_libraries(FCT PUBLIC glfw)
    
    find_package(unofficial-omniverse-physx-sdk CONFIG REQUIRED)
    target_link_libraries(FCT PUBLIC unofficial::omniverse-physx-sdk::sdk)
    
    # Optional: import the defined target to copy over the GPU acceleration libraries (3rd party provided by NVIDIA)
    if(TARGET unofficial::omniverse-physx-sdk::gpu-library)
        if(UNIX)
            # Add rpath setting to find .so libraries on unix based systems
            set_target_properties(FCT PROPERTIES
                BUILD_WITH_INSTALL_RPATH TRUE
                INSTALL_RPATH "$ORIGIN"
            )
        endif()
        add_custom_command(TARGET FCT POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        $<TARGET_FILE:unofficial::omniverse-physx-sdk::gpu-library>
                        $<TARGET_FILE_DIR:FCT>)
        if(WIN32)
            add_custom_command(TARGET FCT POST_BUILD
                            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                            $<TARGET_FILE:unofficial::omniverse-physx-sdk::gpu-device-library>
                            $<TARGET_FILE_DIR:FCT>)
        endif()
    else()
        message(WARNING "GPU acceleration library target not defined - GPU acceleration will NOT be available!")
    endif()
endif()


find_package(unofficial-shaderc CONFIG REQUIRED)
find_package(spirv_cross_core CONFIG REQUIRED)
find_package(spirv_cross_glsl CONFIG REQUIRED)
find_package(spirv_cross_hlsl CONFIG REQUIRED)
find_package(spirv_cross_cpp CONFIG REQUIRED)
find_package(spirv_cross_reflect CONFIG REQUIRED)
target_link_libraries(FCT PUBLIC
        spirv-cross-core
        spirv-cross-glsl
        spirv-cross-hlsl
        spirv-cross-cpp
        spirv-cross-reflect
        unofficial::shaderc::shaderc
)

#target_link_libraries(FCT PUBLIC
#    harfbuzz::harfbuzz
#    harfbuzz::harfbuzz-subset
#    Freetype::Freetype
#    ${Boost_LIBRARIES}
#    ${BULLET_LIBRARIES}
#)
