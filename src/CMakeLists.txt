include_directories(".")

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
)
