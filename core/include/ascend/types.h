// Ascend — podstawowe typy rdzenia: identyfikatory, czas, enumy, stałe protokołu.
//
// Reguła rdzenia: ŻADNYCH zależności od Androida/JNI ani od systemu plików.
// Czas jest zawsze reprezentowany jako milisekundy epoki Unix (UTC).
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <cstdint>

namespace ascend {

// Identyfikatory encji (monotonicznie rosnące w obrębie jednego magazynu).
using HabitId = std::uint64_t;
using EntryId = std::uint64_t;
using CardId = std::uint64_t;

// Czas w milisekundach od epoki Unix (UTC).
using EpochMs = std::int64_t;

// Numer dnia od epoki Unix (UTC) — używany do „dni" w nawykach (streak'i).
using DayNumber = std::int64_t;

constexpr std::int64_t kMillisPerDay = 86'400'000;

// Wersja protokołu kanału sterującego (JNI command) oraz formatu zapisu.
constexpr std::int32_t kProtocolVersion = 1;
constexpr std::int32_t kSaveSchemaVersion = 1;

// Nastrój wpisu dziennika (1..5).
enum class Mood : std::int32_t {
    Awful = 1,
    Bad = 2,
    Neutral = 3,
    Good = 4,
    Great = 5,
};

// Dzień tygodnia (indeks 0..6, ISO-8601: 0 = poniedziałek, 6 = niedziela).
// Używany w masce bitowej harmonogramu nawyku.
enum class Weekday : std::int32_t {
    Monday = 0,
    Tuesday = 1,
    Wednesday = 2,
    Thursday = 3,
    Friday = 4,
    Saturday = 5,
    Sunday = 6,
};

// Konwertuje czas epoki [ms] na numer dnia (UTC). Dla ujemnych wartości
// zaokrągla w dół (prawdziwa podłoga), a nie w stronę zera.
inline DayNumber dayNumberUtc(EpochMs ms) {
    if (ms >= 0) return ms / kMillisPerDay;
    return (ms - (kMillisPerDay - 1)) / kMillisPerDay;
}

}  // namespace ascend
