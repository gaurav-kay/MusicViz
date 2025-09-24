cmake_minimum_required(VERSION 3.22)
project(MusicViz)

set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#include(FetchContent)
#FetchContent_Declare(SFML
#        GIT_REPOSITORY https://github.com/SFML/SFML.git
#        GIT_TAG 3.0.2
#        GIT_SHALLOW ON
#        EXCLUDE_FROM_ALL
#        SYSTEM)
#FetchContent_MakeAvailable(SFML)

add_executable(MusicViz src/fft_tests.cpp)
#target_compile_features(MusicViz PRIVATE cxx_std_17)
#target_link_libraries(MusicViz PRIVATE SFML::Graphics)

find_package(SFML 2.5 COMPONENTS audio system window REQUIRED)
target_link_libraries(MusicViz sfml-audio sfml-system sfml-window fftw3)

# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/SFML-3.0.2/include)
# target_link_libraries (MusicViz
#         ${CMAKE_CURRENT_SOURCE_DIR}/libs/SFML-3.0.2/lib/libsfml-audio.a
#         ${CMAKE_CURRENT_SOURCE_DIR}/libs/SFML-3.0.2/lib/libsfml-system.a
# )
#or
#target_link_libraries (MusicViz ${CMAKE_CURRENT_SOURCE_DIR}/libs/SFML-2.5.1/lib)
