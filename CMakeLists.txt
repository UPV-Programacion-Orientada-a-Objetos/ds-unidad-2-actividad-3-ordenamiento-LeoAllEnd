cmake_minimum_required(VERSION 4.0)
project(Practica3)

set(CMAKE_CXX_STANDARD 20)

add_executable(Practica3 main.cpp
        CircularBuffer.cpp
        CircularBuffer.h
        FileSource.cpp
        FileSource.h
        SerialSource.cpp
        SerialSource.h
        DataSource.h)
