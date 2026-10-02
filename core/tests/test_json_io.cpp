// Testy serializacji magazynu do/z JSON (format zapisu v2).
// SPDX-License-Identifier: GPL-3.0-or-later
#include <gtest/gtest.h>

#include <string>

#include "ascend/json_io.h"
#include "ascend/store.h"

using namespace ascend;

namespace {

Store makeSample() {
    Store s;

    Session a;
    a.startedAtMs = 12345;
    a.durationSeconds = 600;
    a.technique = Technique::BodyScan;
    a.patternId = 0;
    a.moodBefore = Mood::Bad;
    a.moodAfter = Mood::Good;
    a.notes = "relaks";
    s.createSession(a);

    BreathingPattern p;
    p.name = "Własny";
    p.inhaleSeconds = 3;
    p.holdInSeconds = 1;
    p.exhaleSeconds = 5;
    p.holdOutSeconds = 0;
    s.createPattern(p);

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
    EXPECT_EQ(t.nextSessionId, s.nextSessionId);
    EXPECT_EQ(t.nextPatternId, s.nextPatternId);
}

TEST(JsonIo, RoundTripPreservesFields) {
    const Store s = makeSample();
    Store t;
    std::string err;
    ASSERT_TRUE(storeFromJson(storeToJson(s), t, err)) << err;

    ASSERT_EQ(t.sessionsRaw().size(), 1u);
    EXPECT_EQ(t.sessionsRaw()[0].technique, Technique::BodyScan);
    EXPECT_EQ(t.sessionsRaw()[0].durationSeconds, 600);
    ASSERT_TRUE(t.sessionsRaw()[0].moodBefore.has_value());
    EXPECT_EQ(*t.sessionsRaw()[0].moodBefore, Mood::Bad);
    ASSERT_TRUE(t.sessionsRaw()[0].moodAfter.has_value());
    EXPECT_EQ(*t.sessionsRaw()[0].moodAfter, Mood::Good);
    EXPECT_EQ(t.sessionsRaw()[0].notes, "relaks");

    ASSERT_EQ(t.patternsRaw().size(), 1u);
    EXPECT_EQ(t.patternsRaw()[0].name, "Własny");
    EXPECT_EQ(t.patternsRaw()[0].inhaleSeconds, 3);
    EXPECT_EQ(t.patternsRaw()[0].holdInSeconds, 1);
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
        R"({"sessions":[{"id":7,"startedAtMs":1,"durationSeconds":60}],"patterns":[]})", t, err))
        << err;
    EXPECT_GE(t.nextSessionId, SessionId{8});
}

TEST(JsonIo, TechniqueStringRoundTrip) {
    EXPECT_EQ(techniqueToString(techniqueFromString("silence")), "silence");
    EXPECT_EQ(techniqueToString(techniqueFromString("body_scan")), "body_scan");
    EXPECT_EQ(techniqueFromString("nieznane"), Technique::Breath);
}
