add_executable(HighFpsDemo
        HighFpsDemo.cpp)

target_link_libraries(HighFpsDemo
        FCT)

add_executable(VertexRender3DDemo
        VertexRender3DDemo.cpp)
target_link_libraries(VertexRender3DDemo
        FCT)
add_executable(UnlimitedScaleVertexRenderDemo
        UnlimitedScaleVertexRenderDemo.cpp)
target_link_libraries(UnlimitedScaleVertexRenderDemo
        FCT
        FCT_IMGUI)
add_executable(RenderGraphDemo
        RenderGraphDemo.cpp)
target_link_libraries(RenderGraphDemo
        FCT
        FCT_IMGUI)