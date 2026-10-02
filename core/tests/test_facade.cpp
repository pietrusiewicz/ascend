// Testy fasady: kanał poleceń JSON + snapshot.
// SPDX-License-Identifier: GPL-3.0-or-later
#include <gtest/gtest.h>

#include <cstdint>
#include <string>

#include <nlohmann/json.hpp>

#include "ascend/facade.h"
#include "ascend/types.h"

using namespace ascend;
using nlohmann::json;

namespace {

json call(Facade& f, const json& req) {
    return json::parse(f.command(req.dump()));
}

}  // namespace

TEST(Facade, PingAndVersion) {
    Facade f;
    EXPECT_TRUE(call(f, {{"op", "ping"}})["pong"].get<bool>());

    const json v = call(f, {{"op", "version"}});
    EXPECT_TRUE(v["ok"].get<bool>());
    EXPECT_EQ(v["protocol"].get<int>(), kProtocolVersion);
    EXPECT_EQ(v["schema"].get<int>(), kSaveSchemaVersion);
    EXPECT_EQ(v["version"].get<std::string>(), "0.1.0");
}

TEST(Facade, HabitLifecycle) {
    Facade f;

    const json c = call(f, {{"op", "habit.create"}, {"name", "Run"}});
    ASSERT_TRUE(c["ok"].get<bool>());
    const HabitId id = c["habit"]["id"].get<HabitId>();
    EXPECT_EQ(id, HabitId{1});

    const json l = call(f, {{"op", "habit.list"}});
    EXPECT_EQ(l["habits"].size(), 1u);

    const json sc = call(f, {{"op", "habit.setCompleted"}, {"id", id}, {"day", 100}, {"done", true}});
    EXPECT_TRUE(sc["ok"].get<bool>());

    const json st = call(f, {{"op", "habit.stats"}, {"id", id}, {"today", 100}});
    EXPECT_EQ(st["currentStreak"].get<std::int64_t>(), 1);
    EXPECT_TRUE(st["doneToday"].get<bool>());
}

TEST(Facade, HabitCreateRequiresName) {
    Facade f;
    const json c = call(f, {{"op", "habit.create"}});
    EXPECT_FALSE(c["ok"].get<bool>());
}

TEST(Facade, JournalLifecycle) {
    Facade f;

    const json c = call(f, {{"op", "journal.create"}, {"title", "hi"}, {"body", "b"}, {"mood", 5}});
    ASSERT_TRUE(c["ok"].get<bool>());
    EXPECT_EQ(c["entry"]["mood"].get<int>(), 5);
    const EntryId id = c["entry"]["id"].get<EntryId>();

    const json l = call(f, {{"op", "journal.list"}});
    EXPECT_EQ(l["entries"].size(), 1u);

    const json rm = call(f, {{"op", "journal.remove"}, {"id", id}});
    EXPECT_TRUE(rm["ok"].get<bool>());
}

TEST(Facade, CardReview) {
    Facade f;

    const json c = call(f, {{"op", "card.create"}, {"deck", "d"}, {"front", "f"}, {"back", "b"}});
    ASSERT_TRUE(c["ok"].get<bool>());
    const CardId id = c["card"]["id"].get<CardId>();

    const json r = call(f, {{"op", "card.review"}, {"id", id}, {"quality", 4}, {"nowMs", 1000}});
    ASSERT_TRUE(r["ok"].get<bool>());
    EXPECT_EQ(r["card"]["repetitions"].get<int>(), 1);
    EXPECT_EQ(r["card"]["dueMs"].get<EpochMs>(), EpochMs{1000 + kMillisPerDay});
}

TEST(Facade, SnapshotRoundTrip) {
    Facade f;
    call(f, {{"op", "habit.create"}, {"name", "X"}});
    const std::string snap = f.snapshotToJson();

    Facade g;
    std::string err;
    ASSERT_TRUE(g.loadSnapshot(snap, err)) << err;
    EXPECT_EQ(g.snapshotToJson(), snap);
}

TEST(Facade, UnknownOp) {
    Facade f;
    EXPECT_FALSE(call(f, {{"op", "nope"}})["ok"].get<bool>());
}

TEST(Facade, MalformedJson) {
    Facade f;
    const json r = json::parse(f.command("not-json"));
    EXPECT_FALSE(r["ok"].get<bool>());
}
