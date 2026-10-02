// Testy statystyk: passa, czasy, liczba sesji.
// SPDX-License-Identifier: GPL-3.0-or-later
#include <gtest/gtest.h>

#include "ascend/stats.h"

using namespace ascend;

namespace {

Session mkSession(EpochMs startMs, std::int64_t seconds) {
    Session s;
    s.startedAtMs = startMs;
    s.durationSeconds = seconds;
    return s;
}

}  // namespace

TEST(Stats, EmptyStore) {
    const MeditationStats st = computeStats({}, dayNumberUtc(0));
    EXPECT_EQ(st.streakDays, 0);
    EXPECT_EQ(st.longestStreak, 0);
    EXPECT_EQ(st.totalSessions, 0);
    EXPECT_EQ(st.totalSeconds, 0);
}

TEST(Stats, TotalsAndToday) {
    const DayNumber today = dayNumberUtc(2 * kMillisPerDay);  // dzień 2
    std::vector<Session> ss;
    ss.push_back(mkSession(2 * kMillisPerDay + 1000, 60));   // dziś
    ss.push_back(mkSession(2 * kMillisPerDay + 5000, 40));   // dziś
    ss.push_back(mkSession(1 * kMillisPerDay, 30));          // wczoraj

    const MeditationStats st = computeStats(ss, today);
    EXPECT_EQ(st.totalSessions, 3);
    EXPECT_EQ(st.totalSeconds, 130);
    EXPECT_EQ(st.sessionsToday, 2);
    EXPECT_EQ(st.secondsToday, 100);
}

TEST(Stats, CurrentStreakCountsConsecutiveDaysBackFromToday) {
    const DayNumber today = dayNumberUtc(5 * kMillisPerDay);
    std::vector<Session> ss;
    ss.push_back(mkSession(5 * kMillisPerDay, 10));  // dziś
    ss.push_back(mkSession(4 * kMillisPerDay, 10));  // -1
    ss.push_back(mkSession(3 * kMillisPerDay, 10));  // -2
    ss.push_back(mkSession(1 * kMillisPerDay, 10));  // -4 (przerwa)
    const MeditationStats st = computeStats(ss, today);
    EXPECT_EQ(st.streakDays, 3);
    EXPECT_EQ(st.longestStreak, 3);
}

TEST(Stats, CurrentStreakAllowsMissingToday) {
    // Jeśli dziś jeszcze nie było sesji, ale wczoraj tak — passa trwa.
    const DayNumber today = dayNumberUtc(5 * kMillisPerDay);
    std::vector<Session> ss;
    ss.push_back(mkSession(4 * kMillisPerDay, 10));  // wczoraj
    ss.push_back(mkSession(3 * kMillisPerDay, 10));  // przedwczoraj
    const MeditationStats st = computeStats(ss, today);
    EXPECT_EQ(st.streakDays, 2);
}

TEST(Stats, LongestStreakFindsHistoricalMaximum) {
    const DayNumber today = dayNumberUtc(10 * kMillisPerDay);
    std::vector<Session> ss;
    // seria 1,2,3 (3 dni), przerwa, seria 6,7,8,9,10 (5 dni)
    ss.push_back(mkSession(1 * kMillisPerDay, 10));
    ss.push_back(mkSession(2 * kMillisPerDay, 10));
    ss.push_back(mkSession(3 * kMillisPerDay, 10));
    ss.push_back(mkSession(6 * kMillisPerDay, 10));
    ss.push_back(mkSession(7 * kMillisPerDay, 10));
    ss.push_back(mkSession(8 * kMillisPerDay, 10));
    ss.push_back(mkSession(9 * kMillisPerDay, 10));
    ss.push_back(mkSession(10 * kMillisPerDay, 10));
    const MeditationStats st = computeStats(ss, today);
    EXPECT_EQ(st.streakDays, 5);
    EXPECT_EQ(st.longestStreak, 5);
}
