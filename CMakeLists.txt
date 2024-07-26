cmake_minimum_required(VERSION 3.21.0)

project(glory-streamer-app
        VERSION 0.0
        DESCRIPTION "My application for my streams!"
        HOMEPAGE_URL "https://github.com/GloryOfNight/glory-streamer-app"
        LANGUAGES CXX C)

include(cmake/ProjectOptions.cmake)
include(cmake/Sanitizers.cmake)
include(cmake/PreventInSourceBuilds.cmake)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(curl REQUIRED)

add_subdirectory(thirdparty)

add_executable(${PROJECT_NAME} "src/main.cxx" "src/api/youtube_api.cxx" "src/core/engine.cxx" "src/actors/components/sprite_component.cxx" "src/actors/components/font_component.cxx" "src/actors/chat_ghost.cxx" "src/subsystems/youtube_manager.cxx" "src/subsystems/timer_manager.cxx" "src/core/actor.cxx" "src/core/actor_component.cxx" "src/subsystems/chat_ghosts_manager.cxx")

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2-static SDL2_image::SDL2_image-static SDL2_ttf::SDL2_ttf-static imgui-static CURL::libcurl_static httplib nlohmann_json)

target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME} PRIVATE src)

enable_sanitizers(${PROJECT_NAME} 
                    ${ENABLE_SANITIZER_ADDRESS} 
                    ${ENABLE_SANITIZER_LEAK} 
                    ${ENABLE_SANITIZER_UNDEFINED} 
                    ${ENABLE_SANITIZER_THREAD} 
                    ${ENABLE_SANITIZER_MEMORY})

install(TARGETS ${PROJECT_NAME})