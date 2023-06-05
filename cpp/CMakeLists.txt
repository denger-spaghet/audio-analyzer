cmake_minimum_required(VERSION 3.9.1)
project(audio_analyzer)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_COMPILER g++)

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}/include)
LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/lib)


add_compile_options(-lrt -lm -lasound -pthread -lfftw3)
add_executable(audio_analyzer main.cc)

TARGET_LINK_LIBRARIES(audio_analyzer libportaudio.so libfftw3.a)