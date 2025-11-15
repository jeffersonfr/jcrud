cmake_minimum_required(VERSION 3.28)

project(jcrud
        LANGUAGES CXX
        VERSION 1.0
)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type")
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        Release
        Debug
)

set(JCRUD_CLIENT "Americanas" CACHE STRING "Cliente do sistema")
set_property(CACHE JCRUD_CLIENT PROPERTY STRINGS
        Americanas
        BomPreco
        HiperMercado
)

add_compile_definitions(Americanas=1)
add_compile_definitions(BomPreco=2)
add_compile_definitions(HiperMercado=3)
add_compile_definitions(JCRUD_CLIENT=${JCRUD_CLIENT})

add_subdirectory(info)
add_subdirectory(thirdparty)
add_subdirectory(config)
add_subdirectory(src)
add_subdirectory(tests)
