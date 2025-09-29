cmake_minimum_required(VERSION 3.22)
project(MusicViz)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

add_executable(MusicViz src/sfml_display.cpp)
# add_executable(MusicViz src/fft_from_file.cpp)

find_package(SFML 2.5 COMPONENTS audio system window graphics REQUIRED)
target_link_libraries(MusicViz sfml-audio sfml-system sfml-window sfml-graphics fftw3)

