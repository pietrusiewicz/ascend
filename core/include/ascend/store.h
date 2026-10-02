// Ascend — magazyn domenowy w pamięci (wszystkie moduły razem).
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

#include "ascend/habit.h"
#include "ascend/journal.h"
#include "ascend/srs.h"
#include "ascend/types.h"

namespace ascend {

struct Store {
    // ---------------------------------------------------------------- Nawyki
    // Tworzy nawyk, nadając mu kolejny identyfikator. Zwraca utworzony obiekt.
    Habit createHabit(Habit h);
    // Aktualizuje istniejący nawyk (wyszukiwany po id). false, gdy brak id.
    bool updateHabit(const Habit& h);
    bool setHabitArchived(HabitId id, bool archived);
    std::optional<Habit> habit(HabitId id) const;
    std::vector<Habit> habits(bool includeArchived) const;

    // Zaznacza/odznacza ukończenie nawyku w dniu `day`. Idempotentne.
    bool setCompleted(HabitId id, DayNumber day, bool done);
    bool isCompleted(HabitId id, DayNumber day) const;
    // Dni (rosnąco), w których nawyk został ukończony.
    std::vector<DayNumber> completions(HabitId id) const;
    // Bieżący streak (liczony od `today` wstecz, włącznie z `today`).
    std::int64_t currentStreak(HabitId id, DayNumber today) const;
    std::int64_t longestStreak(HabitId id) const;

    // -------------------------------------------------------------- Dziennik
    EntryId createEntry(JournalEntry e);
    bool updateEntry(const JournalEntry& e);
    bool removeEntry(EntryId id);
    std::optional<JournalEntry> entry(EntryId id) const;
    // Wszystkie wpisy, malejąco wg createdAtMs.
    std::vector<JournalEntry> entries() const;

    // --------------------------------------------------------- Nauka (SRS)
    CardId createCard(Card c);
    bool updateCard(const Card& c);
    bool removeCard(CardId id);
    std::optional<Card> card(CardId id) const;
    std::vector<Card> cards() const;
    // Wykonuje krok SM-2 i zwraca zaktualizowaną kartę (nullopt, gdy brak id).
    std::optional<Card> review(CardId id, int quality, EpochMs nowMs);
    // Karty do powtórki (dueMs <= nowMs), posortowane rosnąco wg dueMs.
    std::vector<Card> dueCards(EpochMs nowMs, std::size_t limit) const;

    // -------------------------------------------------- Stan surowy (JSON)
    std::vector<Habit>& habitsRaw() { return mHabits; }
    const std::vector<Habit>& habitsRaw() const { return mHabits; }
    std::vector<JournalEntry>& entriesRaw() { return mEntries; }
    const std::vector<JournalEntry>& entriesRaw() const { return mEntries; }
    std::vector<Card>& cardsRaw() { return mCards; }
    const std::vector<Card>& cardsRaw() const { return mCards; }
    std::vector<Completion>& completionsRaw() { return mCompletions; }
    const std::vector<Completion>& completionsRaw() const { return mCompletions; }

    // Kolejne identyfikatory do nadania (serializowane, by uniknąć kolizji).
    HabitId nextHabitId = 1;
    EntryId nextEntryId = 1;
    CardId nextCardId = 1;

private:
    std::vector<Habit> mHabits;
    std::vector<JournalEntry> mEntries;
    std::vector<Card> mCards;
    std::vector<Completion> mCompletions;
};

}  // namespace ascend
