// Testy encji Session i BreathingPattern.
// SPDX-License-Identifier: GPL-3.0-or-later
#include <gtest/gtest.h>

#include "ascend/breathing.h"
#include "ascend/session.h"

using namespace ascend;

TEST(Session, EndedAtMsComputedFromStartAndDuration) {
    Session s;
    s.startedAtMs = 1000;
    s.durationSeconds = 60;
    EXPECT_EQ(s.endedAtMs(), EpochMs{1000 + 60 * 1000});
}

TEST(BreathingPattern, CycleSecondsSum) {
    BreathingPattern p;
    p.inhaleSeconds = 4;
    p.holdInSeconds = 7;
    p.exhaleSeconds = 8;
    p.holdOutSeconds = 0;
    EXPECT_EQ(p.cycleSeconds(), 19);
}

TEST(BreathingPattern, BuiltinPatternsPresent) {
    const auto patterns = builtinPatterns();
    ASSERT_GE(patterns.size(), 4u);
    // Wszystkie wbudowane mają id != 0 i flagę builtin.
    for (const auto& p : patterns) {
        EXPECT_NE(p.id, PatternId{0});
        EXPECT_TRUE(p.builtin);
        EXPECT_GT(p.inhaleSeconds, 0);
        EXPECT_GT(p.exhaleSeconds, 0);
    }
}
