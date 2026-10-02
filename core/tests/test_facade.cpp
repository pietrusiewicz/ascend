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
}

TEST(Facade, SessionLifecycle) {
    Facade f;

    const json c = call(f, {{"op", "session.create"},
                            {"startedAtMs", 1000},
                            {"durationSeconds", 300},
                            {"technique", "breath"},
                            {"moodBefore", 3},
                            {"moodAfter", 4}});
    ASSERT_TRUE(c["ok"].get<bool>());
    const SessionId id = c["session"]["id"].get<SessionId>();
    EXPECT_EQ(id, SessionId{1});
    EXPECT_EQ(c["session"]["technique"].get<std::string>(), "breath");

    const json l = call(f, {{"op", "session.list"}});
    EXPECT_EQ(l["sessions"].size(), 1u);

    const json rm = call(f, {{"op", "session.remove"}, {"id", id}});
    EXPECT_TRUE(rm["ok"].get<bool>());
}

TEST(Facade, SessionCreateRequiresPositiveDuration) {
    Facade f;
    const json c = call(f, {{"op", "session.create"}, {"durationSeconds", 0}});
    EXPECT_FALSE(c["ok"].get<bool>());
}

TEST(Facade, PatternLifecycle) {
    Facade f;

    const json list0 = call(f, {{"op", "pattern.list"}});
    ASSERT_GE(list0["patterns"].size(), 4u);  // wbudowane

    const json c = call(f, {{"op", "pattern.create"},
                            {"name", "Rytm"},
                            {"inhaleSeconds", 3},
                            {"exhaleSeconds", 6}});
    ASSERT_TRUE(c["ok"].get<bool>());
    const PatternId id = c["pattern"]["id"].get<PatternId>();
    EXPECT_GE(id, PatternId{100});

    // wbudowanych nie usuwamy
    const json rmBuiltin = call(f, {{"op", "pattern.remove"}, {"id", 1}});
    EXPECT_FALSE(rmBuiltin["ok"].get<bool>());
}

TEST(Facade, StatsCommand) {
    Facade f;
    call(f, {{"op", "session.create"},
             {"startedAtMs", kMillisPerDay},
             {"durationSeconds", 120}});

    const json st = call(f, {{"op", "stats"}, {"today", dayNumberUtc(kMillisPerDay)}});
    ASSERT_TRUE(st["ok"].get<bool>());
    EXPECT_EQ(st["totalSessions"].get<std::int64_t>(), 1);
    EXPECT_EQ(st["totalSeconds"].get<std::int64_t>(), 120);
    EXPECT_EQ(st["streakDays"].get<std::int64_t>(), 1);
}

TEST(Facade, SnapshotRoundTrip) {
    Facade f;
    call(f, {{"op", "session.create"}, {"startedAtMs", 1000}, {"durationSeconds", 60}});
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
