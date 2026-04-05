cmake_minimum_required(VERSION 3.22)
project(MusicViz)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

add_executable(MusicViz src/main.cpp)

find_package(SFML 3 COMPONENTS REQUIRED Audio System Window Graphics)
target_link_libraries(MusicViz SFML::Audio SFML::System SFML::Window SFML::Graphics fftw3)
