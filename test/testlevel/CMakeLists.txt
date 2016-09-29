file(GLOB SRCS *.cpp)

add_executable(testlevel ${SRCS})

target_link_libraries(testlevel
    VMAGC
    libgtest
    )

add_test(level_test testlevel)
