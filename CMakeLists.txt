cmake_minimum_required(VERSION 3.31)
project(c_lion C)

set(CMAKE_C_STANDARD 23)

# Compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wswitch-enum -ggdb")

# Include and link directories
set(STEMMER_INCLUDE_DIR "libstemmer_c/include")
set(STEMMER_LIB_DIR "libstemmer_c/lib")

include_directories(${STEMMER_INCLUDE_DIR})
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/jim
        ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
)
link_directories(${STEMMER_LIB_DIR})

# Main app target
add_executable(c_lion
        main.c
        counter.c
        counter.h
        str.c
        str.h
        file_util.c
        file_util.h
        jim/jim.h
        jim/jim1.h
        jim/jim2.h
        jim/jimp.h
)

# Link with stemmer
target_link_libraries(c_lion stemmer)
