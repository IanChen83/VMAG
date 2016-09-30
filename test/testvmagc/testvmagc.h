#ifndef VMAGC_TEST_HEADER
#define VMAGC_TEST_HEADER
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "vmagc.cpp"

class VmagcTest : public ::testing::Test {
    public:
        vmagc::vmagc vm;
        VmagcTest();
};

#endif  // VMAGC_TEST_HEADER
