// Punkt wejścia testów rdzenia (GTest::gtest_main dostarcza main()).
// Ten plik rejestruje globalne środowisko testowe.
// SPDX-License-Identifier: GPL-3.0-or-later
#include <gtest/gtest.h>

namespace {
class CoreEnvironment : public ::testing::Environment {
public:
    void SetUp() override {}
    void TearDown() override {}
};
const bool kRegistered = [] {
    ::testing::AddGlobalTestEnvironment(new CoreEnvironment());
    return true;
}();
[[maybe_unused]] const bool kUnused = kRegistered;
}  // namespace
