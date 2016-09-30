file(GLOB SRCS *.cpp)

add_executable(testvmagc ${SRCS})

target_link_libraries(testvmagc
    VMAGC
    libgtest
    )

add_test(vmagc_test testvmagc)
