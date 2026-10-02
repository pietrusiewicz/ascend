// Ascend — moduł sesji: encja Session (jedna sesja medytacji).
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "ascend/types.h"

namespace ascend {

// Pojedyncza zakończona sesja medytacji.
struct Session {
    SessionId id = 0;
    EpochMs startedAtMs = 0;        // początek sesji (UTC)
    std::int64_t durationSeconds = 0;  // faktyczny czas trwania [s] (> 0)
    Technique technique = Technique::Breath;
    PatternId patternId = 0;        // 0 = bez wzorca oddechu
    std::optional<Mood> moodBefore; // nastrój przed (opcjonalnie)
    std::optional<Mood> moodAfter;  // nastrój po (opcjonalnie)
    std::string notes;              // refleksje / notatki

    // Koniec sesji (start + czas trwania).
    EpochMs endedAtMs() const {
        return startedAtMs + durationSeconds * 1000;
    }
};

}  // namespace ascend
