// Ascend — statystyki medytacji (passa, czasy).
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ascend/stats.h"

#include <algorithm>
#include <set>

namespace ascend {

MeditationStats computeStats(const std::vector<Session>& sessions, DayNumber today) {
    MeditationStats out;

    // Dni, w których była przynajmniej jedna sesja.
    std::set<DayNumber> days;
    for (const auto& s : sessions) {
        ++out.totalSessions;
        out.totalSeconds += s.durationSeconds;

        const DayNumber d = dayNumberUtc(s.startedAtMs);
        days.insert(d);

        if (d == today) {
            ++out.sessionsToday;
            out.secondsToday += s.durationSeconds;
        }
    }

    if (!days.empty()) {
        // Najdłuższa passa: przechodzimy po posortowanych dniach.
        std::int64_t longest = 1;
        std::int64_t run = 1;
        auto it = days.begin();
        DayNumber prev = *it;
        ++it;
        for (; it != days.end(); ++it) {
            if (*it == prev + 1) {
                ++run;
            } else {
                run = 1;
            }
            longest = std::max(longest, run);
            prev = *it;
        }
        out.longestStreak = longest;

        // Bieżąca passa: od `today` wstecz (dni z rzędu). Jeśli dziś nie było
        // jeszcze sesji, passa nadal „żyje", gdy wczoraj była — dzień się nie
        // skończył, więc liczymy od wczoraj.
        DayNumber d = today;
        if (days.count(d) == 0) d = today - 1;
        while (days.count(d) > 0) {
            ++out.streakDays;
            --d;
        }
    }

    return out;
}

}  // namespace ascend
