add_executable(${PROJECT_NAME}
        main.cpp
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
            info
            thirdparty
            jdb::core
            jdb::thirdparty
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
            .
)

# add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Werror -Wnrvo -Wpadded -pedantic)
target_compile_options(${PROJECT_NAME}
        PRIVATE
            -Wall -Werror -Wpedantic -Wconversion -Werror -pedantic
)
