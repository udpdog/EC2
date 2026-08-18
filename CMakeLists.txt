cmake_minimum_required(VERSION 3.20)
project(ec2 LANGUAGES CXX)

add_executable(ec2
    src/main.cpp
    src/cli.cpp
    src/conversion_options.cpp
    src/formats.cpp
    src/imagemagick.cpp
    src/process.cpp
)
target_compile_features(ec2 PRIVATE cxx_std_17)

if(WIN32)
    target_compile_definitions(ec2 PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX)
endif()

if(MSVC)
    target_compile_options(ec2 PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(ec2 PRIVATE -Wall -Wextra -Wpedantic)
endif()
