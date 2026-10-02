// Testy modułu nawyków: harmonogram (weekdayMask) i streak'i.
// SPDX-License-Identifier: GPL-3.0-or-later
#include <gtest/gtest.h>

#include "ascend/habit.h"
#include "ascend/store.h"
#include "ascend/types.h"

using namespace ascend;

TEST(Habit, ScheduledOnDaily) {
    Habit h;
    h.daily = true;
    for (int i = 0; i < 7; ++i) {
        EXPECT_TRUE(h.scheduledOn(static_cast<Weekday>(i)));
    }
}

TEST(Habit, ScheduledOnWeeklyMask) {
    Habit h;
    h.daily = false;
    h.weekdayMask = 0b0001010;  // wtorek (bit 1) i czwartek (bit 3)

    EXPECT_FALSE(h.scheduledOn(Weekday::Monday));
    EXPECT_TRUE(h.scheduledOn(Weekday::Tuesday));
    EXPECT_FALSE(h.scheduledOn(Weekday::Wednesday));
    EXPECT_TRUE(h.scheduledOn(Weekday::Thursday));
    EXPECT_FALSE(h.scheduledOn(Weekday::Friday));
    EXPECT_FALSE(h.scheduledOn(Weekday::Saturday));
    EXPECT_FALSE(h.scheduledOn(Weekday::Sunday));
}

TEST(Habit, CurrentStreak) {
    Store s;
    const Habit h = s.createHabit(Habit{});

    s.setCompleted(h.id, 100, true);
    s.setCompleted(h.id, 101, true);
    s.setCompleted(h.id, 102, true);

    // Dziś odhaczone → streak 3.
    EXPECT_EQ(s.currentStreak(h.id, 102), 3);
    // Dziś (104) i wczoraj (103) puste → streak 0.
    EXPECT_EQ(s.currentStreak(h.id, 104), 0);
    // Dziś (103) jeszcze nieodhaczone, ale wczoraj (102) tak → streak wciąż żyje.
    EXPECT_EQ(s.currentStreak(h.id, 103), 3);
}

TEST(Habit, LongestStreak) {
    Store s;
    const Habit h = s.createHabit(Habit{});

    s.setCompleted(h.id, 100, true);
    s.setCompleted(h.id, 101, true);
    s.setCompleted(h.id, 102, true);
    s.setCompleted(h.id, 105, true);  // przerwa w 103–104

    EXPECT_EQ(s.longestStreak(h.id), 3);
}

TEST(Habit, DayNumberUtc) {
    EXPECT_EQ(dayNumberUtc(0), 0);
    EXPECT_EQ(dayNumberUtc(kMillisPerDay), 1);
    EXPECT_EQ(dayNumberUtc(kMillisPerDay - 1), 0);
}
