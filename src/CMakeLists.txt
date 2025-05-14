# add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Werror -Wnrvo -Wpadded -pedantic)
add_compile_options(-Wall -Werror -Wpedantic -Wconversion -Werror -pedantic)

include_directories(".")

find_package(Boost REQUIRED
  COMPONENTS
    date_time
    filesystem
    iostreams
    thread
)

add_executable(${PROJECT_NAME}
  main.cpp
)

target_link_libraries(${PROJECT_NAME}
    jmixin
    jinject
    fmt
    SQLiteCpp
    sqlite3
    expected
    crypt
    Boost::date_time
    Boost::filesystem
    Boost::iostreams
    Boost::thread
    ftxui::screen
    ftxui::dom
    ftxui::component
)
