// Ascend — implementacja magazynu domenowego (CRUD + streak'i + SRS).
// SPDX-License-Identifier: GPL-3.0-or-later
#include "ascend/store.h"

#include <algorithm>
#include <set>
#include <utility>

namespace ascend {

// ------------------------------------------------------------------ Nawyki
Habit Store::createHabit(Habit h) {
    h.id = nextHabitId++;
    mHabits.push_back(h);
    return h;
}

bool Store::updateHabit(const Habit& h) {
    const auto it = std::find_if(mHabits.begin(), mHabits.end(),
                                 [&](const Habit& x) { return x.id == h.id; });
    if (it == mHabits.end()) return false;
    *it = h;
    return true;
}

bool Store::setHabitArchived(HabitId id, bool archived) {
    const auto it = std::find_if(mHabits.begin(), mHabits.end(),
                                 [&](const Habit& x) { return x.id == id; });
    if (it == mHabits.end()) return false;
    it->archived = archived;
    return true;
}

std::optional<Habit> Store::habit(HabitId id) const {
    const auto it = std::find_if(mHabits.begin(), mHabits.end(),
                                 [&](const Habit& x) { return x.id == id; });
    if (it == mHabits.end()) return std::nullopt;
    return *it;
}

std::vector<Habit> Store::habits(bool includeArchived) const {
    std::vector<Habit> out;
    out.reserve(mHabits.size());
    for (const auto& h : mHabits) {
        if (includeArchived || !h.archived) out.push_back(h);
    }
    return out;
}

bool Store::setCompleted(HabitId id, DayNumber day, bool done) {
    if (!habit(id).has_value()) return false;
    const auto it = std::find_if(mCompletions.begin(), mCompletions.end(),
                                 [&](const Completion& c) {
                                     return c.habitId == id && c.day == day;
                                 });
    if (done) {
        if (it == mCompletions.end()) mCompletions.push_back({id, day});
    } else {
        if (it != mCompletions.end()) mCompletions.erase(it);
    }
    return true;
}

bool Store::isCompleted(HabitId id, DayNumber day) const {
    return std::any_of(mCompletions.begin(), mCompletions.end(),
                       [&](const Completion& c) {
                           return c.habitId == id && c.day == day;
                       });
}

std::vector<DayNumber> Store::completions(HabitId id) const {
    std::vector<DayNumber> out;
    for (const auto& c : mCompletions) {
        if (c.habitId == id) out.push_back(c.day);
    }
    std::sort(out.begin(), out.end());
    return out;
}

std::int64_t Store::currentStreak(HabitId id, DayNumber today) const {
    const auto days = completions(id);
    const std::set<DayNumber> done(days.begin(), days.end());

    DayNumber cursor = today;
    // Jeśli dziś jeszcze nie odhaczono, streak wciąż „żyje", gdy wczoraj było ok.
    if (done.count(cursor) == 0) cursor = today - 1;

    std::int64_t streak = 0;
    while (done.count(cursor) > 0) {
        ++streak;
        --cursor;
    }
    return streak;
}

std::int64_t Store::longestStreak(HabitId id) const {
    const auto days = completions(id);
    if (days.empty()) return 0;
    std::int64_t best = 1;
    std::int64_t run = 1;
    for (std::size_t i = 1; i < days.size(); ++i) {
        if (days[i] == days[i - 1] + 1) {
            ++run;
        } else {
            run = 1;
        }
        best = std::max(best, run);
    }
    return best;
}

// ----------------------------------------------------------------- Dziennik
EntryId Store::createEntry(JournalEntry e) {
    e.id = nextEntryId++;
    if (e.editedAtMs == 0) e.editedAtMs = e.createdAtMs;
    const EntryId id = e.id;
    mEntries.push_back(std::move(e));
    return id;
}

bool Store::updateEntry(const JournalEntry& e) {
    const auto it = std::find_if(mEntries.begin(), mEntries.end(),
                                 [&](const JournalEntry& x) { return x.id == e.id; });
    if (it == mEntries.end()) return false;
    JournalEntry updated = e;
    if (updated.editedAtMs == 0) updated.editedAtMs = updated.createdAtMs;
    *it = std::move(updated);
    return true;
}

bool Store::removeEntry(EntryId id) {
    const auto before = mEntries.size();
    mEntries.erase(std::remove_if(mEntries.begin(), mEntries.end(),
                                  [&](const JournalEntry& x) { return x.id == id; }),
                   mEntries.end());
    return mEntries.size() != before;
}

std::optional<JournalEntry> Store::entry(EntryId id) const {
    const auto it = std::find_if(mEntries.begin(), mEntries.end(),
                                 [&](const JournalEntry& x) { return x.id == id; });
    if (it == mEntries.end()) return std::nullopt;
    return *it;
}

std::vector<JournalEntry> Store::entries() const {
    auto out = mEntries;
    std::sort(out.begin(), out.end(), [](const JournalEntry& a, const JournalEntry& b) {
        if (a.createdAtMs != b.createdAtMs) return a.createdAtMs > b.createdAtMs;
        return a.id > b.id;
    });
    return out;
}

// ----------------------------------------------------------------- Nauka (SRS)
CardId Store::createCard(Card c) {
    c.id = nextCardId++;
    mCards.push_back(std::move(c));
    return mCards.back().id;
}

bool Store::updateCard(const Card& c) {
    const auto it = std::find_if(mCards.begin(), mCards.end(),
                                 [&](const Card& x) { return x.id == c.id; });
    if (it == mCards.end()) return false;
    *it = c;
    return true;
}

bool Store::removeCard(CardId id) {
    const auto before = mCards.size();
    mCards.erase(std::remove_if(mCards.begin(), mCards.end(),
                                [&](const Card& x) { return x.id == id; }),
                 mCards.end());
    return mCards.size() != before;
}

std::optional<Card> Store::card(CardId id) const {
    const auto it = std::find_if(mCards.begin(), mCards.end(),
                                 [&](const Card& x) { return x.id == id; });
    if (it == mCards.end()) return std::nullopt;
    return *it;
}

std::vector<Card> Store::cards() const {
    return mCards;
}

std::optional<Card> Store::review(CardId id, int quality, EpochMs nowMs) {
    const auto it = std::find_if(mCards.begin(), mCards.end(),
                                 [&](const Card& x) { return x.id == id; });
    if (it == mCards.end()) return std::nullopt;
    *it = sm2Review(*it, quality, nowMs);
    return *it;
}

std::vector<Card> Store::dueCards(EpochMs nowMs, std::size_t limit) const {
    std::vector<Card> out;
    for (const auto& c : mCards) {
        if (c.dueMs <= nowMs) out.push_back(c);
    }
    std::sort(out.begin(), out.end(), [](const Card& a, const Card& b) {
        return a.dueMs < b.dueMs;
    });
    if (out.size() > limit) out.resize(limit);
    return out;
}

}  // namespace ascend
