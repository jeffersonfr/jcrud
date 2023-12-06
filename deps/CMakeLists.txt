include(FetchContent)

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

