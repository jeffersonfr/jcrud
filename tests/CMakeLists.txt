include(FetchContent)

FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.11.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

option(INSTALL_GTEST "" OFF)
option(INSTALL_GMOCK "" OFF)

FetchContent_MakeAvailable(googletest)

macro(module_test)
    set(id ${ARGV0}_test)
    add_executable(${id} ${id}.cpp)
    add_test(${id} ${id} COMMAND $<TARGET_FILE:${id}>)
    target_link_libraries(${id}
            PRIVATE
                info
                thirdparty
                gtest_main
    )
    target_include_directories(${id}
            PRIVATE
                ${PROJECT_SOURCE_DIR}/src
    )
    target_include_directories(${PROJECT_NAME}
            PRIVATE
                .
                ${CMAKE_BINARY_DIR}
    )
    unset(id)
endmacro()

enable_testing()

module_test(api_v1_unit)
