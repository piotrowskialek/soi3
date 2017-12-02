cmake_minimum_required(VERSION 3.9)
project(soi3)

set(CMAKE_CXX_STANDARD 11)

add_executable(soi3 main.cpp model_sources/Solution.cpp model_headers/Solution.h model_headers/dto/Variable.h model_headers/dto/Data.h)