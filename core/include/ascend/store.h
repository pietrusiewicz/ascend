// Ascend — magazyn domenowy w pamięci (sesje + wzorce oddechu).
//
// Rdzeń nie czyta dysku ani sieci. Serializację zapewnia json_io,
// a trwałość (zapis do pliku/preferencji) — warstwa platformowa.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

#include "ascend/breathing.h"
#include "ascend/session.h"
#include "ascend/stats.h"
#include "ascend/types.h"

namespace ascend {

struct Store {
    // ------------------------------------------------------- Sesje
    SessionId createSession(Session s);
    bool updateSession(const Session& s);
    bool removeSession(SessionId id);
    std::optional<Session> session(SessionId id) const;
    // Wszystkie sesje, malejąco wg startedAtMs.
    std::vector<Session> sessions() const;

    // ------------------------------------------------------- Wzorce oddechu
    PatternId createPattern(BreathingPattern p);
    bool updatePattern(const BreathingPattern& p);
    bool removePattern(PatternId id);
    std::optional<BreathingPattern> pattern(PatternId id) const;
    // Wbudowane wzorce najpierw (w kolejności), potem własne (wg id).
    std::vector<BreathingPattern> patterns() const;

    // ------------------------------------------------------- Statystyki
    MeditationStats stats(DayNumber today) const;

    // ---------------------------------------------- Stan surowy (JSON)
    std::vector<Session>& sessionsRaw() { return mSessions; }
    const std::vector<Session>& sessionsRaw() const { return mSessions; }
    std::vector<BreathingPattern>& patternsRaw() { return mPatterns; }
    const std::vector<BreathingPattern>& patternsRaw() const { return mPatterns; }

    // Kolejne identyfikatory do nadania (serializowane, by uniknąć kolizji).
    SessionId nextSessionId = 1;
    // Id wzorców wbudowanych zaczynają się od 1..N; własne od kNextCustomPatternId.
    PatternId nextPatternId = 100;

private:
    std::vector<Session> mSessions;
    std::vector<BreathingPattern> mPatterns;
};

}  // namespace ascend
