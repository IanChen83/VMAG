file(GLOB SRCS *.cpp)

add_executable(testknapsack ${SRCS})

target_link_libraries(testknapsack
    VMAGC
    libgtest
    )

add_test(knapsack_test testknapsack)
