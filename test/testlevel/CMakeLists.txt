file(GLOB SRCS *.cpp)

add_executable(testlevel ${SRCS})

target_link_libraries(testlevel
    VMAGC
    libgtest
    )

install(TARGETS testlevel DESTINATION bin)
add_test(level_test testlevel)
