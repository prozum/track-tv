cmake_minimum_required(VERSION 2.8)

project(track-tv CXX C)

find_package( OpenCV REQUIRED )

include_directories (${CMAKE_SOURCE_DIR}/firmata)
add_subdirectory(firmata)
link_directories (${CMAKE_BINARY_DIR}/firmata)

add_executable(
    track-tv 
    main.cpp
)

target_link_libraries( track-tv ${OpenCV_LIBS} Firmata)

install(TARGETS track-tv DESTINATION bin)
