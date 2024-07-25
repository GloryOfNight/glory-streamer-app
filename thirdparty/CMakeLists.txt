add_subdirectory(cpp-httplib)
add_subdirectory(nlohmann_json)

#### ImGUI begin ####
add_library(imgui-static STATIC imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/backends/imgui_impl_sdl2.cpp imgui/backends/imgui_impl_sdlrenderer2.cpp)
target_include_directories(imgui-static PUBLIC imgui imgui/backends)
target_link_libraries(imgui-static PRIVATE SDL2::SDL2-static)
#### ImGUI end ####