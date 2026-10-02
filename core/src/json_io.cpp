// Ascend — serializacja magazynu do/z JSON (format zapisu v1).
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

nlohmann::json habitToJson(const Habit& h) {
    return json{
        {"id", h.id},
        {"name", h.name},
        {"description", h.description},
        {"icon", h.icon},
        {"color", h.color},
        {"daily", h.daily},
        {"weekdayMask", h.weekdayMask},
        {"createdAtMs", h.createdAtMs},
        {"archived", h.archived},
    };
}

nlohmann::json entryToJson(const JournalEntry& e) {
    return json{
        {"id", e.id},
        {"createdAtMs", e.createdAtMs},
        {"editedAtMs", e.editedAtMs},
        {"title", e.title},
        {"body", e.body},
        {"mood", moodToInt(e.mood)},
    };
}

nlohmann::json cardToJson(const Card& c) {
    return json{
        {"id", c.id},
        {"deck", c.deck},
        {"front", c.front},
        {"back", c.back},
        {"dueMs", c.dueMs},
        {"intervalDays", c.intervalDays},
        {"ease", c.ease},
        {"repetitions", c.repetitions},
        {"lapses", c.lapses},
    };
}

std::string storeToJson(const Store& store) {
    json j;
    j["schema"] = kSaveSchemaVersion;
    j["protocol"] = kProtocolVersion;
    j["nextHabitId"] = store.nextHabitId;
    j["nextEntryId"] = store.nextEntryId;
    j["nextCardId"] = store.nextCardId;

    j["habits"] = json::array();
    for (const auto& h : store.habitsRaw()) j["habits"].push_back(habitToJson(h));

    j["completions"] = json::array();
    for (const auto& c : store.completionsRaw()) {
        j["completions"].push_back(json{{"habitId", c.habitId}, {"day", c.day}});
    }

    j["journal"] = json::array();
    for (const auto& e : store.entriesRaw()) j["journal"].push_back(entryToJson(e));

    j["cards"] = json::array();
    for (const auto& c : store.cardsRaw()) j["cards"].push_back(cardToJson(c));

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
    out.nextHabitId = num<HabitId>(j, "nextHabitId", 1);
    out.nextEntryId = num<EntryId>(j, "nextEntryId", 1);
    out.nextCardId = num<CardId>(j, "nextCardId", 1);

    if (j.contains("habits") && j["habits"].is_array()) {
        for (const auto& h : j["habits"]) {
            Habit x;
            x.id = num<HabitId>(h, "id", 0);
            x.name = h.value("name", std::string{});
            x.description = h.value("description", std::string{});
            x.icon = h.value("icon", std::string{"check"});
            x.color = num<std::uint32_t>(h, "color", 0xFF4F46E5u);
            x.daily = h.value("daily", true);
            x.weekdayMask = static_cast<std::uint8_t>(
                num<std::uint32_t>(h, "weekdayMask", 0x7Fu));
            x.createdAtMs = num<EpochMs>(h, "createdAtMs", 0);
            x.archived = h.value("archived", false);
            out.habitsRaw().push_back(std::move(x));
        }
    }
    if (j.contains("completions") && j["completions"].is_array()) {
        for (const auto& c : j["completions"]) {
            Completion x;
            x.habitId = num<HabitId>(c, "habitId", 0);
            x.day = num<DayNumber>(c, "day", 0);
            out.completionsRaw().push_back(x);
        }
    }
    if (j.contains("journal") && j["journal"].is_array()) {
        for (const auto& e : j["journal"]) {
            JournalEntry x;
            x.id = num<EntryId>(e, "id", 0);
            x.createdAtMs = num<EpochMs>(e, "createdAtMs", 0);
            x.editedAtMs = num<EpochMs>(e, "editedAtMs", 0);
            x.title = e.value("title", std::string{});
            x.body = e.value("body", std::string{});
            x.mood = moodFromInt(static_cast<int>(num<std::int64_t>(e, "mood", 3)));
            out.entriesRaw().push_back(std::move(x));
        }
    }
    if (j.contains("cards") && j["cards"].is_array()) {
        for (const auto& c : j["cards"]) {
            Card x;
            x.id = num<CardId>(c, "id", 0);
            x.deck = c.value("deck", std::string{});
            x.front = c.value("front", std::string{});
            x.back = c.value("back", std::string{});
            x.dueMs = num<EpochMs>(c, "dueMs", 0);
            x.intervalDays = c.value("intervalDays", 0.0);
            x.ease = c.value("ease", 2.5);
            x.repetitions = static_cast<std::int32_t>(
                num<std::int64_t>(c, "repetitions", 0));
            x.lapses = static_cast<std::int32_t>(
                num<std::int64_t>(c, "lapses", 0));
            out.cardsRaw().push_back(std::move(x));
        }
    }

    // Bezpieczeństwo: kolejne id zawsze większe niż największe istniejące,
    // nawet gdy plik został ręcznie edytowany.
    for (const auto& h : out.habitsRaw()) out.nextHabitId = std::max(out.nextHabitId, h.id + 1);
    for (const auto& e : out.entriesRaw()) out.nextEntryId = std::max(out.nextEntryId, e.id + 1);
    for (const auto& c : out.cardsRaw()) out.nextCardId = std::max(out.nextCardId, c.id + 1);

    store = std::move(out);
    return true;
}

}  // namespace ascend
