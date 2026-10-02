// Ascend — statystyki medytacji: passa, czasy, liczba sesji.
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <cstdint>
#include <vector>

#include "ascend/session.h"
#include "ascend/types.h"

namespace ascend {

// Podsumowanie statystyk (liczone na żądanie z listy sesji).
struct MeditationStats {
    std::int64_t streakDays = 0;     // dni z rzędu (od `today` wstecz) z sesją
    std::int64_t longestStreak = 0;  // najdłuższa passa w historii
    std::int64_t totalSessions = 0;  // łączna liczba sesji
    std::int64_t totalSeconds = 0;   // łączny czas medytacji [s]
    std::int64_t sessionsToday = 0;  // sesje w dniu `today`
    std::int64_t secondsToday = 0;   // czas medytacji dziś [s]
};

// Oblicza statystyki z listy sesji. `today` = numer dnia (dayNumberUtc).
MeditationStats computeStats(const std::vector<Session>& sessions, DayNumber today);

}  // namespace ascend
