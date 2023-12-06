cmake_minimum_required(VERSION 3.15)

project(jcrud
  LANGUAGES CXX
  VERSION 1.0
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(deps)
add_subdirectory(src)
