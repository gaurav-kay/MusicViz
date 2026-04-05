cmake_minimum_required(VERSION 3.22)
project(MusicViz)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)

add_executable(MusicViz src/main.cpp)

# find_package(SFML 2.5 COMPONENTS audio system window graphics REQUIRED)
# target_link_libraries(MusicViz sfml-audio sfml-system sfml-window sfml-graphics fftw3)
find_package(SFML 3 COMPONENTS REQUIRED Audio System Window Graphics)
target_link_libraries(MusicViz SFML::Audio SFML::System SFML::Window SFML::Graphics fftw3)

find_package(Threads REQUIRED)
target_link_libraries(MusicViz Threads::Threads)

