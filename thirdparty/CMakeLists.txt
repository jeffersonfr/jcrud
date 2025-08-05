include(FetchContent)

find_package(Boost REQUIRED
    COMPONENTS
        date_time
        filesystem
        iostreams
        thread
)

# jmixin
FetchContent_Declare(
    jmixin
    GIT_REPOSITORY https://github.com/jeffersonfr/jmixin.git
)
FetchContent_MakeAvailable(jmixin)

# jinject
FetchContent_Declare(
    jinject
    GIT_REPOSITORY https://github.com/jeffersonfr/jinject.git
    GIT_TAG main
)
FetchContent_MakeAvailable(jinject)

# fmt
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
)
FetchContent_MakeAvailable(fmt)

# SQLiteCpp
FetchContent_Declare(
    SQLiteCpp
    GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
)
FetchContent_MakeAvailable(SQLiteCpp)

# CrowCpp
# dependencies:
#  - sudo apt-get install libasio-dev
#  - cmake-build-debug/_deps/crowcpp-src/include/crow/settings.h:
#    >> comment the line '#define CROW_ENABLE_LOGGING'
set(ASIO_INCLUDE_DIR "/usr/include")

set(CROW_ENABLE_SSL ON)
set(CROW_ENABLE_LOGGING OFF)

FetchContent_Declare(
    CrowCpp
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG master
    PATCH_COMMAND git apply --stat --quiet ${CMAKE_CURRENT_SOURCE_DIR}/patches/crowcpp-logging.patch
)
FetchContent_MakeAvailable(CrowCpp)

# FTXUI
FetchContent_Declare(
    FTXUI
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
    GIT_TAG main
)
FetchContent_MakeAvailable(FTXUI)

# jwt-cpp
set(JWT_BUILD_EXAMPLES OFF)
FetchContent_Declare(
    JwtCpp
    GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
    GIT_TAG master
)
FetchContent_MakeAvailable(JwtCpp)

# expected
FetchContent_Declare(
    expected
    GIT_REPOSITORY https://github.com/TartanLlama/expected.git
)
FetchContent_MakeAvailable(expected)

FetchContent_GetProperties(expected)
if (expected_POPULATED)
    message("SOURCE_DIR: ${expected_SOURCE_DIR}")
    message("BINARY_DIR: ${expected_BINARY_DIR}")
endif ()

# create a target to use in src/
add_library(thirdparty INTERFACE)

target_link_libraries(thirdparty
    INTERFACE
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
        Crow::Crow
        ftxui::screen
        ftxui::dom
        ftxui::component
        jwt-cpp
)
