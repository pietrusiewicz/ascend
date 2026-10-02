// Ascend — moduł nawyków: encja Habit i rekord ukończenia Completion.
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <cstdint>
#include <string>

#include "ascend/types.h"

namespace ascend {

// Jeden nawyk do śledzenia.
struct Habit {
    HabitId id = 0;
    std::string name;
    std::string description;
    std::string icon = "check";         // klucz ikony (warstwa UI mapuje na zasób)
    std::uint32_t color = 0xFF4F46E5;   // ARGB; domyślnie indygo
    bool daily = true;                  // true = codziennie; false = wg weekdayMask
    std::uint8_t weekdayMask = 0x7F;    // bity 0..6 = pon..ndz, gdy !daily
    EpochMs createdAtMs = 0;
    bool archived = false;

    // Czy nawyk jest zaplanowany na dany dzień tygodnia?
    bool scheduledOn(Weekday w) const {
        if (daily) return true;
        const auto bit = static_cast<unsigned>(w);
        return bit < 7 && ((weekdayMask >> bit) & 0x1u) != 0;
    }
};

// Rekord ukończenia nawyku w konkretnym dniu (UTC).
struct Completion {
    HabitId habitId = 0;
    DayNumber day = 0;
};

}  // namespace ascend
