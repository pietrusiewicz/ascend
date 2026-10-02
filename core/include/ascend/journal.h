// Ascend — moduł dziennika: encja JournalEntry.
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <string>

#include "ascend/types.h"

namespace ascend {

// Pojedynczy wpis dziennika.
struct JournalEntry {
    EntryId id = 0;
    EpochMs createdAtMs = 0;
    EpochMs editedAtMs = 0;
    std::string title;
    std::string body;
    Mood mood = Mood::Neutral;
};

}  // namespace ascend
