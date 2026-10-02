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
using SessionId = std::uint64_t;
using PatternId = std::uint64_t;

// Czas w milisekundach od epoki Unix (UTC).
using EpochMs = std::int64_t;

// Numer dnia od epoki Unix (UTC) — używany do liczenia „dni" (passa medytacji).
using DayNumber = std::int64_t;

constexpr std::int64_t kMillisPerDay = 86'400'000;

// Wersja protokołu kanału sterującego (JNI command) oraz formatu zapisu.
constexpr std::int32_t kProtocolVersion = 2;
constexpr std::int32_t kSaveSchemaVersion = 2;

// Nastrój (1..5) — używany przed/po sesji medytacji.
enum class Mood : std::int32_t {
    Awful = 1,
    Bad = 2,
    Neutral = 3,
    Good = 4,
    Great = 5,
};

// Technika medytacji. W JSON jako string (snake_case).
enum class Technique : std::int32_t {
    Breath = 0,    // oddech (wzorzec oddechu)
    Silence = 1,   // cisza
    BodyScan = 2,  // skan ciała
    Walking = 3,   // medytacja w ruchu
    Mantra = 4,    // mantra
    Guided = 5,    // z przewodnikiem
};

// Konwertuje czas epoki [ms] na numer dnia (UTC). Dla ujemnych wartości
// zaokrągla w dół (prawdziwa podłoga), a nie w stronę zera.
inline DayNumber dayNumberUtc(EpochMs ms) {
    if (ms >= 0) return ms / kMillisPerDay;
    return (ms - (kMillisPerDay - 1)) / kMillisPerDay;
}

}  // namespace ascend
