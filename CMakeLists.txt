cmake_minimum_required(VERSION 3.18)

project(concurrency
    LANGUAGES C
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

include_directories(common)

add_executable(cafe
    cafe/main.c
    common/barrier.c
)
# target_compile_definitions(cafe PRIVATE SIM PRIORITY)

add_executable(parlour
    parlour/main.c
    parlour/util.c
    common/barrier.c
)
# target_compile_definitions(parlour PRIVATE SIM PRIORITY)
