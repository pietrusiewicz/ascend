// Testy serializacji magazynu do/z JSON (format zapisu v1).
// SPDX-License-Identifier: GPL-3.0-or-later
#include <gtest/gtest.h>

#include <string>

#include "ascend/json_io.h"
#include "ascend/store.h"

using namespace ascend;

namespace {

Store makeSample() {
    Store s;

    Habit h;
    h.name = "Read";
    h.icon = "book";
    h.color = 0xFF00FF00u;
    h.daily = false;
    h.weekdayMask = 0b0001010;
    h.createdAtMs = 12345;
    s.createHabit(h);
    s.setCompleted(1, 100, true);
    s.setCompleted(1, 101, true);

    JournalEntry e;
    e.title = "Day 1";
    e.body = "hello";
    e.mood = Mood::Great;
    e.createdAtMs = 999;
    s.createEntry(e);

    Card c;
    c.deck = "eng";
    c.front = "kot";
    c.back = "cat";
    c.dueMs = 123;
    c.intervalDays = 3.5;
    c.ease = 2.4;
    c.repetitions = 2;
    c.lapses = 1;
    s.createCard(c);

    return s;
}

}  // namespace

TEST(JsonIo, RoundTripIsStable) {
    const Store s = makeSample();
    const std::string json = storeToJson(s);

    Store t;
    std::string err;
    ASSERT_TRUE(storeFromJson(json, t, err)) << err;

    EXPECT_EQ(storeToJson(t), json);
    EXPECT_EQ(t.nextHabitId, s.nextHabitId);
    EXPECT_EQ(t.nextEntryId, s.nextEntryId);
    EXPECT_EQ(t.nextCardId, s.nextCardId);
}

TEST(JsonIo, RoundTripPreservesFields) {
    const Store s = makeSample();
    Store t;
    std::string err;
    ASSERT_TRUE(storeFromJson(storeToJson(s), t, err)) << err;

    ASSERT_EQ(t.habitsRaw().size(), 1u);
    EXPECT_EQ(t.habitsRaw()[0].name, "Read");
    EXPECT_EQ(t.habitsRaw()[0].icon, "book");
    EXPECT_EQ(t.habitsRaw()[0].color, 0xFF00FF00u);
    EXPECT_EQ(t.habitsRaw()[0].weekdayMask, 0b0001010);
    EXPECT_EQ(t.habitsRaw()[0].createdAtMs, 12345);

    ASSERT_EQ(t.completionsRaw().size(), 2u);

    ASSERT_EQ(t.entriesRaw().size(), 1u);
    EXPECT_EQ(t.entriesRaw()[0].title, "Day 1");
    EXPECT_EQ(t.entriesRaw()[0].mood, Mood::Great);

    ASSERT_EQ(t.cardsRaw().size(), 1u);
    EXPECT_DOUBLE_EQ(t.cardsRaw()[0].intervalDays, 3.5);
    EXPECT_DOUBLE_EQ(t.cardsRaw()[0].ease, 2.4);
    EXPECT_EQ(t.cardsRaw()[0].repetitions, 2);
}

TEST(JsonIo, RejectsUnsupportedSchema) {
    Store t;
    std::string err;
    EXPECT_FALSE(storeFromJson(R"({"schema":99})", t, err));
    EXPECT_FALSE(err.empty());
}

TEST(JsonIo, RejectsMalformedJson) {
    Store t;
    std::string err;
    EXPECT_FALSE(storeFromJson("not json", t, err));
    EXPECT_FALSE(err.empty());
}

TEST(JsonIo, BumpsNextIdsFromContent) {
    Store t;
    std::string err;
    ASSERT_TRUE(storeFromJson(
        R"({"habits":[{"id":7,"name":"x"}],"journal":[],"cards":[]})", t, err))
        << err;
    EXPECT_GE(t.nextHabitId, HabitId{8});
}
