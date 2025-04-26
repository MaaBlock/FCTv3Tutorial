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

add_library(FCT_IMGUI STATIC
        headers.cpp
        ${IMGUI_SOURCES}
)

target_include_directories(FCT_IMGUI PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends
)

find_package(Vulkan REQUIRED)
target_link_libraries(FCT_IMGUI PRIVATE FCT
        ${Vulkan_LIBRARIES}
)

