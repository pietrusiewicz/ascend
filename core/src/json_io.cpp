// Ascend — serializacja magazynu do/z JSON (format zapisu v2).
// SPDX-License-Identifier: GPL-3.0-or-later
#include "ascend/json_io.h"

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>

namespace ascend {
using nlohmann::json;

namespace {

// Odczyt liczby z JSON (akceptuje signed i unsigned) z wartością domyślną.
template <typename T>
T num(const json& j, const char* key, T def) {
    if (!j.contains(key)) return def;
    const auto& v = j[key];
    if (v.is_number_integer()) return static_cast<T>(v.get<std::int64_t>());
    if (v.is_number_unsigned()) return static_cast<T>(v.get<std::uint64_t>());
    return def;
}

int moodToInt(Mood m) {
    return static_cast<int>(m);
}

Mood moodFromInt(int v) {
    switch (v) {
        case 1: return Mood::Awful;
        case 2: return Mood::Bad;
        case 3: return Mood::Neutral;
        case 4: return Mood::Good;
        case 5: return Mood::Great;
        default: return Mood::Neutral;
    }
}

}  // namespace

std::string techniqueToString(Technique t) {
    switch (t) {
        case Technique::Breath: return "breath";
        case Technique::Silence: return "silence";
        case Technique::BodyScan: return "body_scan";
        case Technique::Walking: return "walking";
        case Technique::Mantra: return "mantra";
        case Technique::Guided: return "guided";
    }
    return "breath";
}

Technique techniqueFromString(std::string_view s) {
    if (s == "silence") return Technique::Silence;
    if (s == "body_scan") return Technique::BodyScan;
    if (s == "walking") return Technique::Walking;
    if (s == "mantra") return Technique::Mantra;
    if (s == "guided") return Technique::Guided;
    return Technique::Breath;
}

nlohmann::json sessionToJson(const Session& s) {
    json j{
        {"id", s.id},
        {"startedAtMs", s.startedAtMs},
        {"durationSeconds", s.durationSeconds},
        {"technique", techniqueToString(s.technique)},
        {"patternId", s.patternId},
        {"notes", s.notes},
    };
    if (s.moodBefore) j["moodBefore"] = moodToInt(*s.moodBefore);
    if (s.moodAfter) j["moodAfter"] = moodToInt(*s.moodAfter);
    return j;
}

nlohmann::json patternToJson(const BreathingPattern& p) {
    return json{
        {"id", p.id},
        {"name", p.name},
        {"inhaleSeconds", p.inhaleSeconds},
        {"holdInSeconds", p.holdInSeconds},
        {"exhaleSeconds", p.exhaleSeconds},
        {"holdOutSeconds", p.holdOutSeconds},
        {"builtin", p.builtin},
    };
}

std::string storeToJson(const Store& store) {
    json j;
    j["schema"] = kSaveSchemaVersion;
    j["protocol"] = kProtocolVersion;
    j["nextSessionId"] = store.nextSessionId;
    j["nextPatternId"] = store.nextPatternId;

    j["sessions"] = json::array();
    for (const auto& s : store.sessionsRaw()) j["sessions"].push_back(sessionToJson(s));

    j["patterns"] = json::array();
    for (const auto& p : store.patternsRaw()) j["patterns"].push_back(patternToJson(p));

    return j.dump();
}

bool storeFromJson(std::string_view raw, Store& store, std::string& err) {
    json j;
    try {
        j = json::parse(raw.begin(), raw.end());
    } catch (const std::exception& e) {
        err = std::string("JSON parse error: ") + e.what();
        return false;
    }
    if (!j.is_object()) {
        err = "root is not an object";
        return false;
    }
    if (j.contains("schema")) {
        const auto schema = num<std::int64_t>(j, "schema", kSaveSchemaVersion);
        if (schema != kSaveSchemaVersion) {
            err = "unsupported save schema " + std::to_string(schema);
            return false;
        }
    }

    Store out;
    out.nextSessionId = num<SessionId>(j, "nextSessionId", 1);
    out.nextPatternId = num<PatternId>(j, "nextPatternId", 100);

    if (j.contains("sessions") && j["sessions"].is_array()) {
        for (const auto& s : j["sessions"]) {
            Session x;
            x.id = num<SessionId>(s, "id", 0);
            x.startedAtMs = num<EpochMs>(s, "startedAtMs", 0);
            x.durationSeconds = num<std::int64_t>(s, "durationSeconds", 0);
            x.technique = techniqueFromString(s.value("technique", std::string{"breath"}));
            x.patternId = num<PatternId>(s, "patternId", 0);
            if (s.contains("moodBefore")) {
                x.moodBefore = moodFromInt(static_cast<int>(num<std::int64_t>(s, "moodBefore", 0)));
            }
            if (s.contains("moodAfter")) {
                x.moodAfter = moodFromInt(static_cast<int>(num<std::int64_t>(s, "moodAfter", 0)));
            }
            x.notes = s.value("notes", std::string{});
            out.sessionsRaw().push_back(std::move(x));
        }
    }
    if (j.contains("patterns") && j["patterns"].is_array()) {
        for (const auto& p : j["patterns"]) {
            BreathingPattern x;
            x.id = num<PatternId>(p, "id", 0);
            x.name = p.value("name", std::string{});
            x.inhaleSeconds = static_cast<std::int32_t>(num<std::int64_t>(p, "inhaleSeconds", 4));
            x.holdInSeconds = static_cast<std::int32_t>(num<std::int64_t>(p, "holdInSeconds", 0));
            x.exhaleSeconds = static_cast<std::int32_t>(num<std::int64_t>(p, "exhaleSeconds", 4));
            x.holdOutSeconds = static_cast<std::int32_t>(num<std::int64_t>(p, "holdOutSeconds", 0));
            x.builtin = p.value("builtin", false);
            out.patternsRaw().push_back(std::move(x));
        }
    }

    // Bezpieczeństwo: kolejne id zawsze większe niż największe istniejące.
    for (const auto& s : out.sessionsRaw()) out.nextSessionId = std::max(out.nextSessionId, s.id + 1);
    for (const auto& p : out.patternsRaw()) {
        if (p.id >= 100) out.nextPatternId = std::max(out.nextPatternId, p.id + 1);
    }

    store = std::move(out);
    return true;
}

}  // namespace ascend
