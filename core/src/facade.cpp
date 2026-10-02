// Ascend — fasada rdzenia: kanał poleceń JSON + trwałość + metadane.
// SPDX-License-Identifier: GPL-3.0-or-later
#include "ascend/facade.h"

#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>

#include <nlohmann/json.hpp>

#include "ascend/json_io.h"

#ifndef ASC_VERSION_STRING
#define ASC_VERSION_STRING "0.2.0"
#endif

namespace ascend {
using nlohmann::json;

namespace {

json err(std::string msg) {
    return json{{"ok", false}, {"error", std::move(msg)}};
}

json okJson() {
    return json{{"ok", true}};
}

json statsJson(const MeditationStats& st, DayNumber today) {
    return json{{"ok", true},
                {"today", today},
                {"streakDays", st.streakDays},
                {"longestStreak", st.longestStreak},
                {"totalSessions", st.totalSessions},
                {"totalSeconds", st.totalSeconds},
                {"sessionsToday", st.sessionsToday},
                {"secondsToday", st.secondsToday}};
}

// Odczyt mood z JSON: klucz może być int (1..5) albo null/brak.
std::optional<Mood> moodField(const json& j, const char* key) {
    if (!j.contains(key) || j[key].is_null()) return std::nullopt;
    const int v = j[key].get<int>();
    switch (v) {
        case 1: return Mood::Awful;
        case 2: return Mood::Bad;
        case 3: return Mood::Neutral;
        case 4: return Mood::Good;
        case 5: return Mood::Great;
        default: return std::nullopt;
    }
}

}  // namespace

std::string Facade::command(std::string_view raw) {
    json j;
    try {
        j = json::parse(raw.begin(), raw.end());
    } catch (const std::exception& e) {
        return err(std::string("parse: ") + e.what()).dump();
    }
    if (!j.is_object()) return err("request is not an object").dump();

    const std::string op = j.value("op", std::string{});
    if (op.empty()) return err("missing 'op'").dump();

    try {
        if (op == "ping") {
            return json{{"ok", true}, {"pong", true}}.dump();
        }

        if (op == "version") {
            return json{{"ok", true},
                        {"version", coreVersion()},
                        {"protocol", protocolVersion()},
                        {"schema", saveSchemaVersion()}}.dump();
        }

        if (op == "stats") {
            const DayNumber today = j.value("today", DayNumber{0});
            return statsJson(mStore.stats(today), today).dump();
        }

        if (op == "overview") {
            const DayNumber today = j.value("today", DayNumber{0});
            return statsJson(mStore.stats(today), today).dump();
        }

        // ------------------------------------------------------- Sesje
        if (op == "session.list") {
            auto arr = json::array();
            for (const auto& s : mStore.sessions()) arr.push_back(sessionToJson(s));
            return json{{"ok", true}, {"sessions", std::move(arr)}}.dump();
        }

        if (op == "session.get") {
            const auto s = mStore.session(j.value("id", SessionId{0}));
            if (!s) return err("session not found").dump();
            return json{{"ok", true}, {"session", sessionToJson(*s)}}.dump();
        }

        if (op == "session.create") {
            Session s;
            s.startedAtMs = j.value("startedAtMs", EpochMs{0});
            s.durationSeconds = j.value("durationSeconds", std::int64_t{0});
            s.technique = techniqueFromString(j.value("technique", std::string{"breath"}));
            s.patternId = j.value("patternId", PatternId{0});
            s.moodBefore = moodField(j, "moodBefore");
            s.moodAfter = moodField(j, "moodAfter");
            s.notes = j.value("notes", std::string{});
            if (s.durationSeconds <= 0) return err("durationSeconds must be > 0").dump();
            const SessionId id = mStore.createSession(std::move(s));
            return json{{"ok", true}, {"session", sessionToJson(*mStore.session(id))}}.dump();
        }

        if (op == "session.update") {
            const auto existing = mStore.session(j.value("id", SessionId{0}));
            if (!existing) return err("session not found").dump();
            Session s = *existing;
            if (j.contains("startedAtMs")) s.startedAtMs = j["startedAtMs"].get<EpochMs>();
            if (j.contains("durationSeconds")) s.durationSeconds = j["durationSeconds"].get<std::int64_t>();
            if (j.contains("technique")) s.technique = techniqueFromString(j["technique"].get<std::string>());
            if (j.contains("patternId")) s.patternId = j["patternId"].get<PatternId>();
            if (j.contains("moodBefore")) s.moodBefore = moodField(j, "moodBefore");
            if (j.contains("moodAfter")) s.moodAfter = moodField(j, "moodAfter");
            if (j.contains("notes")) s.notes = j["notes"].get<std::string>();
            mStore.updateSession(s);
            return json{{"ok", true}, {"session", sessionToJson(*mStore.session(s.id))}}.dump();
        }

        if (op == "session.remove") {
            if (!mStore.removeSession(j.value("id", SessionId{0}))) {
                return err("session not found").dump();
            }
            return okJson().dump();
        }

        // -------------------------------------------------- Wzorce oddechu
        if (op == "pattern.list") {
            auto arr = json::array();
            for (const auto& p : mStore.patterns()) arr.push_back(patternToJson(p));
            return json{{"ok", true}, {"patterns", std::move(arr)}}.dump();
        }

        if (op == "pattern.create") {
            BreathingPattern p;
            p.name = j.value("name", std::string{});
            p.inhaleSeconds = j.value("inhaleSeconds", 4);
            p.holdInSeconds = j.value("holdInSeconds", 0);
            p.exhaleSeconds = j.value("exhaleSeconds", 4);
            p.holdOutSeconds = j.value("holdOutSeconds", 0);
            if (p.name.empty()) return err("name is required").dump();
            if (p.inhaleSeconds <= 0 || p.exhaleSeconds <= 0) {
                return err("inhaleSeconds and exhaleSeconds must be > 0").dump();
            }
            const PatternId id = mStore.createPattern(std::move(p));
            return json{{"ok", true}, {"pattern", patternToJson(*mStore.pattern(id))}}.dump();
        }

        if (op == "pattern.update") {
            const auto existing = mStore.pattern(j.value("id", PatternId{0}));
            if (!existing) return err("pattern not found").dump();
            BreathingPattern p = *existing;
            if (j.contains("name")) p.name = j["name"].get<std::string>();
            if (j.contains("inhaleSeconds")) p.inhaleSeconds = j["inhaleSeconds"].get<std::int32_t>();
            if (j.contains("holdInSeconds")) p.holdInSeconds = j["holdInSeconds"].get<std::int32_t>();
            if (j.contains("exhaleSeconds")) p.exhaleSeconds = j["exhaleSeconds"].get<std::int32_t>();
            if (j.contains("holdOutSeconds")) p.holdOutSeconds = j["holdOutSeconds"].get<std::int32_t>();
            mStore.updatePattern(p);
            return json{{"ok", true}, {"pattern", patternToJson(*mStore.pattern(p.id))}}.dump();
        }

        if (op == "pattern.remove") {
            if (!mStore.removePattern(j.value("id", PatternId{0}))) {
                return err("pattern not found or builtin").dump();
            }
            return okJson().dump();
        }
    } catch (const std::exception& e) {
        return err(std::string("command failed: ") + e.what()).dump();
    }

    return err("unknown op: " + op).dump();
}

std::string Facade::snapshotToJson() const {
    return storeToJson(mStore);
}

bool Facade::loadSnapshot(std::string_view raw, std::string& err) {
    return storeFromJson(raw, mStore, err);
}

std::int32_t Facade::protocolVersion() {
    return kProtocolVersion;
}

std::int32_t Facade::saveSchemaVersion() {
    return kSaveSchemaVersion;
}

std::string Facade::coreVersion() {
    return ASC_VERSION_STRING;
}

}  // namespace ascend
