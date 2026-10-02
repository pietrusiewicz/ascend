// Testy magazynu: nadawanie id, aktualizacja, archiwizacja, ukończenia, SRS.
// SPDX-License-Identifier: GPL-3.0-or-later
#include <gtest/gtest.h>

#include "ascend/store.h"

using namespace ascend;

TEST(Store, HabitIdAssignment) {
    Store s;
    Habit a;
    a.name = "a";
    Habit b;
    b.name = "b";
    EXPECT_EQ(s.createHabit(a).id, HabitId{1});
    EXPECT_EQ(s.createHabit(b).id, HabitId{2});
}

TEST(Store, HabitUpdateAndArchive) {
    Store s;
    Habit h = s.createHabit(Habit{});
    h.name = "x";
    EXPECT_TRUE(s.updateHabit(h));
    ASSERT_TRUE(s.habit(h.id).has_value());
    EXPECT_EQ(s.habit(h.id)->name, "x");

    Habit unknown;
    unknown.id = 999;
    EXPECT_FALSE(s.updateHabit(unknown));

    EXPECT_TRUE(s.setHabitArchived(h.id, true));
    EXPECT_TRUE(s.habit(h.id)->archived);
    EXPECT_TRUE(s.habits(false).empty());
    EXPECT_EQ(s.habits(true).size(), 1u);
}

TEST(Store, CompletionsAreIdempotent) {
    Store s;
    const Habit h = s.createHabit(Habit{});

    EXPECT_TRUE(s.setCompleted(h.id, 5, true));
    EXPECT_TRUE(s.isCompleted(h.id, 5));
    EXPECT_TRUE(s.setCompleted(h.id, 5, true));  // ponownie → bez duplikatu
    EXPECT_EQ(s.completions(h.id).size(), 1u);

    EXPECT_TRUE(s.setCompleted(h.id, 5, false));
    EXPECT_FALSE(s.isCompleted(h.id, 5));
    EXPECT_EQ(s.completions(h.id).size(), 0u);

    EXPECT_FALSE(s.setCompleted(999, 5, true));  // nieznany nawyk
}

TEST(Store, DueCardsSortedByDueDate) {
    Store s;

    Card a;
    a.front = "a";
    a.back = "1";
    a.dueMs = 100;
    Card b;
    b.front = "b";
    b.back = "2";
    b.dueMs = 50;
    Card c;
    c.front = "c";
    c.back = "3";
    c.dueMs = 500;
    s.createCard(a);
    s.createCard(b);
    s.createCard(c);

    const auto due = s.dueCards(100, 10);
    ASSERT_EQ(due.size(), 2u);
    EXPECT_EQ(due[0].front, "b");  // najwcześniejszy termin pierwszy
    EXPECT_EQ(due[1].front, "a");
}

TEST(Store, ReviewUnknownCard) {
    Store s;
    EXPECT_FALSE(s.review(1, 4, 0).has_value());
}

TEST(Store, CardUpdateAndRemove) {
    Store s;
    Card c;
    c.front = "f";
    c.back = "b";
    const CardId id = s.createCard(c);

    Card u;
    u.id = id;
    u.front = "f2";
    u.back = "b";
    EXPECT_TRUE(s.updateCard(u));
    ASSERT_TRUE(s.card(id).has_value());
    EXPECT_EQ(s.card(id)->front, "f2");

    EXPECT_TRUE(s.removeCard(id));
    EXPECT_FALSE(s.card(id).has_value());
}
