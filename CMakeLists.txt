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

add_executable(${PROJECT_NAME} "src/main.cxx")

target_link_libraries(${PROJECT_NAME} PRIVATE SDL3-static imgui-static CURL::libcurl)

enable_sanitizers(${PROJECT_NAME} 
                    ${ENABLE_SANITIZER_ADDRESS} 
                    ${ENABLE_SANITIZER_LEAK} 
                    ${ENABLE_SANITIZER_UNDEFINED} 
                    ${ENABLE_SANITIZER_THREAD} 
                    ${ENABLE_SANITIZER_MEMORY})

install(TARGETS ${PROJECT_NAME})