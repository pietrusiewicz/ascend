// Testy magazynu: sesje, wzorce oddechu, statystyki.
// SPDX-License-Identifier: GPL-3.0-or-later
#include <gtest/gtest.h>

#include "ascend/store.h"

using namespace ascend;

TEST(Store, SessionIdAssignment) {
    Store s;
    Session a;
    a.startedAtMs = 1;
    a.durationSeconds = 60;
    Session b;
    b.startedAtMs = 2;
    b.durationSeconds = 60;
    EXPECT_EQ(s.createSession(a), SessionId{1});
    EXPECT_EQ(s.createSession(b), SessionId{2});
}

TEST(Store, SessionsSortedDescByStart) {
    Store s;
    Session a;
    a.startedAtMs = 100;
    a.durationSeconds = 10;
    Session b;
    b.startedAtMs = 200;
    b.durationSeconds = 10;
    Session c;
    c.startedAtMs = 300;
    c.durationSeconds = 10;
    s.createSession(a);
    s.createSession(c);
    s.createSession(b);

    const auto out = s.sessions();
    ASSERT_EQ(out.size(), 3u);
    EXPECT_EQ(out[0].startedAtMs, EpochMs{300});
    EXPECT_EQ(out[1].startedAtMs, EpochMs{200});
    EXPECT_EQ(out[2].startedAtMs, EpochMs{100});
}

TEST(Store, SessionUpdateAndRemove) {
    Store s;
    Session x;
    x.startedAtMs = 1;
    x.durationSeconds = 30;
    const SessionId id = s.createSession(x);

    Session u = *s.session(id);
    u.durationSeconds = 90;
    u.notes = "spokój";
    EXPECT_TRUE(s.updateSession(u));
    ASSERT_TRUE(s.session(id).has_value());
    EXPECT_EQ(s.session(id)->durationSeconds, 90);
    EXPECT_EQ(s.session(id)->notes, "spokój");

    EXPECT_TRUE(s.removeSession(id));
    EXPECT_FALSE(s.session(id).has_value());
    EXPECT_FALSE(s.updateSession(u));
}

TEST(Store, PatternsIncludeBuiltinsFirst) {
    Store s;
    const auto all = s.patterns();
    ASSERT_GE(all.size(), 4u);
    EXPECT_TRUE(all[0].builtin);
    EXPECT_EQ(all[0].id, PatternId{1});
}

TEST(Store, CustomPatternLifecycle) {
    Store s;
    BreathingPattern p;
    p.name = "Mój rytm";
    p.inhaleSeconds = 3;
    p.exhaleSeconds = 6;
    const PatternId id = s.createPattern(p);
    EXPECT_GE(id, PatternId{100});
    ASSERT_TRUE(s.pattern(id).has_value());
    EXPECT_FALSE(s.pattern(id)->builtin);

    // update
    BreathingPattern u = *s.pattern(id);
    u.exhaleSeconds = 7;
    EXPECT_TRUE(s.updatePattern(u));
    EXPECT_EQ(s.pattern(id)->exhaleSeconds, 7);

    // wbudowanych nie można usunąć
    EXPECT_FALSE(s.removePattern(1));
    // własny usuwa się
    EXPECT_TRUE(s.removePattern(id));
    EXPECT_FALSE(s.pattern(id).has_value());
}

TEST(Store, StatsDelegatesToComputeStats) {
    Store s;
    Session a;
    a.startedAtMs = kMillisPerDay;
    a.durationSeconds = 60;
    s.createSession(a);

    const DayNumber today = dayNumberUtc(kMillisPerDay);
    const MeditationStats st = s.stats(today);
    EXPECT_EQ(st.totalSessions, 1);
    EXPECT_EQ(st.streakDays, 1);
    EXPECT_EQ(st.secondsToday, 60);
}
