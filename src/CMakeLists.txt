# add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Werror -Wnrvo -Wpadded -pedantic)
add_compile_options(-Wall -Werror -Wpedantic -Wconversion -Werror -pedantic)

include_directories(".")

add_executable(${PROJECT_NAME}
  main.cpp
)

target_link_libraries(${PROJECT_NAME}
    info
    thirdparty
)
