# Music Visualizer

A simple Music Visualizer (like the ones on YouTube) that shows the Magnitude Spectrum over Mel frequencies written in C++. 

![Music Visualizer](audio/output.gif)

## Description

This project uses SFML 3 to read audio files, and performs a simple FFT every 20ms or so, to compute the frequency domain magnitude of the music sample. The linear frequency spectrum is then converted to the Mel scale to better represent human hearing. The resulting magnitudes are binned and plotted against Mel frequency bands to generate the visualization

## Setup

### Dependencies
- FFTW3
- C++20
- SFML 3.0.5 (Note the installation directory if built from source)
- CMake and Make

### Instructions
- Clone the repo
- Set the filename to be read in `main.cpp`
- `cmake -B build -DCMAKE_PREFIX_PATH=<SFML_PATH>`
- `cd build`
- `make`
- `./MusicViz`

### Improvements
- File selection
- Smoother lines
- Add weighted movement of the lines, and use RGB for different weights
- Normalize Y axis