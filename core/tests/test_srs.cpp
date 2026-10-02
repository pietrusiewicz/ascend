// Testy algorytmu SM-2 (powtórki rozłożone w czasie).
// SPDX-License-Identifier: GPL-3.0-or-later
#include <gtest/gtest.h>

#include <cmath>

#include "ascend/srs.h"

using namespace ascend;

TEST(Srs, Sm2Progression) {
    const EpochMs t0 = 1000;
    Card c;
    c.ease = 2.5;
    c.intervalDays = 0.0;
    c.repetitions = 0;

    // 1. powtórka (q=4): interwał 1 dzień, EF bez zmian (2.5).
    c = sm2Review(c, 4, t0);
    EXPECT_EQ(c.repetitions, 1);
    EXPECT_DOUBLE_EQ(c.intervalDays, 1.0);
    EXPECT_DOUBLE_EQ(c.ease, 2.5);
    EXPECT_EQ(c.dueMs, t0 + kMillisPerDay);

    // 2. powtórka (q=4): interwał 6 dni.
    c = sm2Review(c, 4, c.dueMs);
    EXPECT_EQ(c.repetitions, 2);
    EXPECT_DOUBLE_EQ(c.intervalDays, 6.0);
    EXPECT_EQ(c.dueMs, t0 + kMillisPerDay + 6 * kMillisPerDay);

    // 3. powtórka (q=4): interwał = round(6 * 2.5) = 15.
    c = sm2Review(c, 4, c.dueMs);
    EXPECT_EQ(c.repetitions, 3);
    EXPECT_DOUBLE_EQ(c.intervalDays, 15.0);

    // 4. powtórka (q=4): interwał = round(15 * 2.5) = 38 (half-away-from-zero).
    c = sm2Review(c, 4, c.dueMs);
    EXPECT_EQ(c.repetitions, 4);
    EXPECT_DOUBLE_EQ(c.intervalDays, 38.0);
    EXPECT_DOUBLE_EQ(c.ease, 2.5);
}

TEST(Srs, Sm2FailureResets) {
    const EpochMs t0 = 2000;
    Card c;
    c.ease = 2.5;
    c.repetitions = 3;
    c.intervalDays = 15.0;

    c = sm2Review(c, 2, t0);

    EXPECT_EQ(c.repetitions, 0);
    EXPECT_EQ(c.lapses, 1);
    EXPECT_DOUBLE_EQ(c.intervalDays, 1.0);
    // EF' = 2.5 + (0.1 - 3*(0.08 + 3*0.02)) = 2.5 - 0.32 = 2.18
    EXPECT_NEAR(c.ease, 2.18, 1e-9);
    EXPECT_EQ(c.dueMs, t0 + kMillisPerDay);
}

TEST(Srs, EaseFloor) {
    Card c;
    c.ease = 1.35;
    for (int i = 0; i < 10; ++i) {
        c = sm2Review(c, 0, 0);
    }
    EXPECT_GE(c.ease, 1.3);
}

TEST(Srs, QualityClamped) {
    const EpochMs t0 = 3000;
    Card c;
    c.ease = 2.5;

    const Card a = sm2Review(c, 99, t0);   // jak q=5
    const Card b = sm2Review(c, 5, t0);    // q=5
    EXPECT_DOUBLE_EQ(a.ease, b.ease);
    EXPECT_EQ(a.repetitions, b.repetitions);
    EXPECT_EQ(a.dueMs, b.dueMs);
}
