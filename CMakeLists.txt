cmake_minimum_required(VERSION 3.31)
project(c_lion C)

set(CMAKE_C_STANDARD 23)

# Replace with your real paths
set(STEMMER_INCLUDE_DIR "libstemmer_c/include")
set(STEMMER_LIB_DIR "libstemmer_c/lib")

include_directories(${STEMMER_INCLUDE_DIR})
link_directories(${STEMMER_LIB_DIR})

add_executable(c_lion
        main.c
        counter.c
        counter.h
        str.c
        str.h
)

# Link the static library
target_link_libraries(c_lion stemmer)
