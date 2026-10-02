// Ascend — moduł oddechu: encja BreathingPattern i wzorce wbudowane.
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "ascend/types.h"

namespace ascend {

// Wzorzec oddechu: cykl wdech → wstrzymanie → wydech → wstrzymanie (w sekundach).
// Każda faza > 0; hold może być 0 (brak wstrzymania).
struct BreathingPattern {
    PatternId id = 0;
    std::string name;
    std::int32_t inhaleSeconds = 4;
    std::int32_t holdInSeconds = 0;
    std::int32_t exhaleSeconds = 4;
    std::int32_t holdOutSeconds = 0;
    bool builtin = false;  // wbudowane wzorce nie są usuwalne

    // Pełny cykl w sekundach.
    std::int32_t cycleSeconds() const {
        return inhaleSeconds + holdInSeconds + exhaleSeconds + holdOutSeconds;
    }
};

// Wbudowane wzorce oddechu (id > 0 zarezerwowane; widoczne w każdej instalacji).
inline std::vector<BreathingPattern> builtinPatterns() {
    return {
        {1, "Spokojny (4-4)", 4, 0, 4, 0, true},
        {2, "Relaksacyjny (4-7-8)", 4, 7, 8, 0, true},
        {3, "Kwadrat (4-4-4-4)", 4, 4, 4, 4, true},
        {4, "Głęboki (5-5)", 5, 0, 5, 0, true},
    };
}

}  // namespace ascend
