cmake_minimum_required(VERSION 3.22.1)
project(aniflip_engine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE ANIFLIP_ENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/jni/*.cpp"
)

add_library(aniflip_engine SHARED ${ANIFLIP_ENGINE_SOURCES})

target_include_directories(aniflip_engine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(ANDROID)
    find_library(log-lib log)
    target_link_libraries(aniflip_engine PRIVATE ${log-lib})
endif()
