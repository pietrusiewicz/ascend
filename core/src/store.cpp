// Ascend — implementacja magazynu domenowego (sesje + wzorce oddechu + statystyki).
// SPDX-License-Identifier: GPL-3.0-or-later
#include "ascend/store.h"

#include <algorithm>
#include <utility>

namespace ascend {

// ------------------------------------------------------------------ Sesje
SessionId Store::createSession(Session s) {
    s.id = nextSessionId++;
    const SessionId id = s.id;
    mSessions.push_back(std::move(s));
    return id;
}

bool Store::updateSession(const Session& s) {
    const auto it = std::find_if(mSessions.begin(), mSessions.end(),
                                 [&](const Session& x) { return x.id == s.id; });
    if (it == mSessions.end()) return false;
    *it = s;
    return true;
}

bool Store::removeSession(SessionId id) {
    const auto before = mSessions.size();
    mSessions.erase(std::remove_if(mSessions.begin(), mSessions.end(),
                                   [&](const Session& x) { return x.id == id; }),
                    mSessions.end());
    return mSessions.size() != before;
}

std::optional<Session> Store::session(SessionId id) const {
    const auto it = std::find_if(mSessions.begin(), mSessions.end(),
                                 [&](const Session& x) { return x.id == id; });
    if (it == mSessions.end()) return std::nullopt;
    return *it;
}

std::vector<Session> Store::sessions() const {
    auto out = mSessions;
    std::sort(out.begin(), out.end(), [](const Session& a, const Session& b) {
        if (a.startedAtMs != b.startedAtMs) return a.startedAtMs > b.startedAtMs;
        return a.id > b.id;
    });
    return out;
}

// ------------------------------------------------------- Wzorce oddechu
PatternId Store::createPattern(BreathingPattern p) {
    p.id = nextPatternId++;
    p.builtin = false;
    mPatterns.push_back(p);
    return p.id;
}

bool Store::updatePattern(const BreathingPattern& p) {
    const auto it = std::find_if(mPatterns.begin(), mPatterns.end(),
                                 [&](const BreathingPattern& x) { return x.id == p.id; });
    if (it == mPatterns.end()) return false;
    BreathingPattern updated = p;
    updated.builtin = it->builtin;  // nie zmieniaj flagi wbudowania
    *it = updated;
    return true;
}

bool Store::removePattern(PatternId id) {
    const auto it = std::find_if(mPatterns.begin(), mPatterns.end(),
                                 [&](const BreathingPattern& x) { return x.id == id; });
    if (it == mPatterns.end() || it->builtin) return false;  // wbudowanych nie usuwamy
    mPatterns.erase(it);
    return true;
}

std::optional<BreathingPattern> Store::pattern(PatternId id) const {
    // Najpierw wbudowane (stałe), potem zapisane własne.
    for (const auto& p : builtinPatterns()) {
        if (p.id == id) return p;
    }
    const auto it = std::find_if(mPatterns.begin(), mPatterns.end(),
                                 [&](const BreathingPattern& x) { return x.id == id; });
    if (it == mPatterns.end()) return std::nullopt;
    return *it;
}

std::vector<BreathingPattern> Store::patterns() const {
    std::vector<BreathingPattern> out = builtinPatterns();
    for (const auto& p : mPatterns) out.push_back(p);
    return out;
}

// ------------------------------------------------------- Statystyki
MeditationStats Store::stats(DayNumber today) const {
    return computeStats(mSessions, today);
}

}  // namespace ascend
