add_library(FCT_IMGUI STATIC
    headers.cpp
)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(FCT_IMGUI PRIVATE FCT imgui::imgui)

