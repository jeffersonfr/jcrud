add_executable(${PROJECT_NAME}
        main.cpp
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
            info
            thirdparty
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
            .
)

set(WARNING_LIST -Wall -Wextra -Wpedantic -Werror -Wconversion -Wpadded -pedantic -Wno-padded -Wno-error=conversion -Wno-error=unused-parameter)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-Wnrvo" COMPILER_HAS_WNRVO)
if(COMPILER_HAS_WRNVO)
    list(APPEND WARNING_LIST -Wnrvo -Wno-nrvo)
endif ()

target_compile_options(${PROJECT_NAME}
        PRIVATE
            ${WARNING_LIST}
)
