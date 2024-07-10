add_subdirectory(SDL)
set(USE_SCHANNEL ON)
set(USE_WINDOWS_SSPI ON)
add_subdirectory(curl)
add_subdirectory(cpp-httplib)
add_subdirectory(nlohmann_json)

#### ImGUI begin ####
add_library(imgui-static STATIC imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/backends/imgui_impl_sdl3.cpp imgui/backends/imgui_impl_sdlrenderer3.cpp)
target_include_directories(imgui-static PUBLIC imgui imgui/backends)
target_link_libraries(imgui-static PUBLIC SDL3-static)
#### ImGUI end ####