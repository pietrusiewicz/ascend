// Testy modułu dziennika: CRUD i sortowanie wpisów.
// SPDX-License-Identifier: GPL-3.0-or-later
#include <gtest/gtest.h>

#include "ascend/journal.h"
#include "ascend/store.h"

using namespace ascend;

TEST(Journal, CreateAssignsIdsAndSortsDesc) {
    Store s;

    JournalEntry a;
    a.title = "a";
    a.createdAtMs = 100;
    a.mood = Mood::Good;

    JournalEntry b;
    b.title = "b";
    b.createdAtMs = 200;
    b.mood = Mood::Bad;

    const EntryId idA = s.createEntry(a);
    const EntryId idB = s.createEntry(b);
    EXPECT_EQ(idA, EntryId{1});
    EXPECT_EQ(idB, EntryId{2});

    const auto entries = s.entries();
    ASSERT_EQ(entries.size(), 2u);
    EXPECT_EQ(entries[0].id, idB);  // najnowszy pierwszy
    EXPECT_EQ(entries[1].id, idA);
}

TEST(Journal, UpdateAndRemove) {
    Store s;

    JournalEntry e;
    e.title = "t";
    e.createdAtMs = 10;
    const EntryId id = s.createEntry(e);

    JournalEntry updated;
    updated.id = id;
    updated.title = "t2";
    updated.createdAtMs = 10;
    updated.editedAtMs = 20;
    updated.mood = Mood::Great;

    EXPECT_TRUE(s.updateEntry(updated));
    ASSERT_TRUE(s.entry(id).has_value());
    EXPECT_EQ(s.entry(id)->title, "t2");
    EXPECT_EQ(s.entry(id)->editedAtMs, 20);

    EXPECT_TRUE(s.removeEntry(id));
    EXPECT_FALSE(s.entry(id).has_value());
    EXPECT_FALSE(s.removeEntry(id));  // drugi raz → false
}

TEST(Journal, UpdateUnknown) {
    Store s;
    JournalEntry e;
    e.id = 999;
    e.createdAtMs = 1;
    EXPECT_FALSE(s.updateEntry(e));
}
