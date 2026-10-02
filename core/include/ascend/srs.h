// Ascend — moduł nauki: powtórki rozłożone w czasie (spaced repetition).
// Implementacja algorytmu SM-2 (SuperMemo-2).
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <cstdint>
#include <string>

#include "ascend/types.h"

namespace ascend {

// Karta powtórkowa (fiszka).
struct Card {
    CardId id = 0;
    std::string deck;
    std::string front;
    std::string back;
    EpochMs dueMs = 0;            // kiedy karta staje się „do powtórki"
    double intervalDays = 0.0;    // obecny interwał w dniach
    double ease = 2.5;            // współczynnik łatwości (EF)
    std::int32_t repetitions = 0; // poprawnych powtórek z rzędu
    std::int32_t lapses = 0;      // liczba zapomnień
};

// Jakość odpowiedzi 0..5 (5 = bezbłędnie, 4 = dobrze, 3 = z trudem,
// 2 = błędnie, 1 = błędnie z zapamiętaną treścią, 0 = całkowite zapomnienie).
//
// Krok SM-2: aktualizuje interwał, EF, licznik powtórek i termin następnej
// powtórki (dueMs = nowMs + interval * 24h). Zwraca zaktualizowaną kartę.
Card sm2Review(const Card& c, int quality, EpochMs nowMs);

}  // namespace ascend
