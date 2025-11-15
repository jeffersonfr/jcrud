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

# add_compile_options(-Wall -Wextra -Wpedantic -Werror -Wconversion -Wnrvo -Wpadded -pedantic)
target_compile_options(${PROJECT_NAME}
        PRIVATE
        -Wall -Wextra -Wpedantic -Werror -Wconversion -Wnrvo -Wpadded -pedantic -Wno-padded -Wno-nrvo -Wno-error=conversion -Wno-error=unused-parameter
)
