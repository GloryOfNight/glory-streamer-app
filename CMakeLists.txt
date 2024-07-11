cmake_minimum_required(VERSION 3.21.0)

project(glory-streamer-app
        VERSION 0.0
        DESCRIPTION "My application for my streams!"
        HOMEPAGE_URL "https://github.com/GloryOfNight/glory-streamer-app"
        LANGUAGES CXX C)

include(cmake/ProjectOptions.cmake)
include(cmake/Sanitizers.cmake)
include(cmake/PreventInSourceBuilds.cmake)

add_subdirectory(thirdparty)

add_executable(${PROJECT_NAME} "src/main.cxx" "src/api/youtube_data_api.cxx" "src/core/engine.cxx" "src/core/sprite_component.cxx" "src/core/font_component.cxx" "src/objects/subscriber_ghost.cxx")

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2-static SDL2_image SDL2_ttf imgui-static CURL::libcurl httplib nlohmann_json)

target_include_directories(${PROJECT_NAME} PUBLIC include)

enable_sanitizers(${PROJECT_NAME} 
                    ${ENABLE_SANITIZER_ADDRESS} 
                    ${ENABLE_SANITIZER_LEAK} 
                    ${ENABLE_SANITIZER_UNDEFINED} 
                    ${ENABLE_SANITIZER_THREAD} 
                    ${ENABLE_SANITIZER_MEMORY})

install(TARGETS ${PROJECT_NAME})