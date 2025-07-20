include(platform/platform.cmake)

# ImGui核心源文件 - 直接使用当前目录下的imgui源码
set(IMGUI_SOURCES
        imgui/imgui.cpp
        imgui/imgui_demo.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_tables.cpp
        imgui/imgui_widgets.cpp
        imgui/backends/imgui_impl_vulkan.cpp
        imgui/backends/imgui_impl_glfw.cpp
)

file(GLOB FCT_IMGUI_HEADERS
        LIST_DIRECTORIES false
        CONFIGURE_DEPENDS
        "./src/*.h"
        "./src/*.hpp"
)

list(FILTER FCT_IMGUI_HEADERS EXCLUDE REGEX ".*FCT_IMGUI\\.h$")

add_library(FCT_IMGUI STATIC
        src/headers.cpp
        ${IMGUI_SOURCES}
)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/FCT_IMGUI)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/FCT_IMGUI.h")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/FCT_IMGUI.h ${CMAKE_CURRENT_SOURCE_DIR}/include/FCT_IMGUI.h COPYONLY)
endif()


foreach(header_file ${FCT_IMGUI_HEADERS})
    file(RELATIVE_PATH relative_path ${CMAKE_CURRENT_SOURCE_DIR}/src ${header_file})
    get_filename_component(header_dir ${relative_path} DIRECTORY)

    if(header_dir)
        file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/FCT_IMGUI/${header_dir})
    endif()

    configure_file(${header_file} ${CMAKE_CURRENT_SOURCE_DIR}/include/FCT_IMGUI/${relative_path} COPYONLY)
endforeach()

target_include_directories(FCT_IMGUI PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

find_package(Vulkan REQUIRED)
target_link_libraries(FCT_IMGUI PRIVATE FCT
        ${Vulkan_LIBRARIES}
)

find_package(Vulkan REQUIRED)
target_link_libraries(FCT_IMGUI PRIVATE FCT
        ${Vulkan_LIBRARIES}
)

