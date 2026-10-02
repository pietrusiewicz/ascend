// Ascend — fasada rdzenia: kanał poleceń JSON + trwałość + metadane.
// SPDX-License-Identifier: GPL-3.0-or-later
#include "ascend/facade.h"

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>

#include <nlohmann/json.hpp>

#include "ascend/json_io.h"

#ifndef ASC_VERSION_STRING
#define ASC_VERSION_STRING "0.1.0"
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

        if (op == "overview") {
            const DayNumber today = j.value("today", DayNumber{0});
            const EpochMs nowMs = today * kMillisPerDay;
            std::size_t active = 0;
            std::size_t doneToday = 0;
            for (const auto& h : mStore.habits(false)) {
                ++active;
                if (mStore.isCompleted(h.id, today)) ++doneToday;
            }
            return json{{"ok", true},
                        {"today", today},
                        {"activeHabits", active},
                        {"doneToday", doneToday},
                        {"dueCards", mStore.dueCards(nowMs, std::size_t(-1)).size()},
                        {"entries", mStore.entries().size()}}.dump();
        }

        // ------------------------------------------------------- Nawyki
        if (op == "habit.list") {
            const bool includeArchived = j.value("includeArchived", false);
            auto arr = json::array();
            for (const auto& h : mStore.habits(includeArchived)) arr.push_back(habitToJson(h));
            return json{{"ok", true}, {"habits", std::move(arr)}}.dump();
        }

        if (op == "habit.get") {
            const auto h = mStore.habit(j.value("id", HabitId{0}));
            if (!h) return err("habit not found").dump();
            return json{{"ok", true}, {"habit", habitToJson(*h)}}.dump();
        }

        if (op == "habit.create") {
            Habit h;
            h.name = j.value("name", std::string{});
            h.description = j.value("description", std::string{});
            h.icon = j.value("icon", std::string{"check"});
            h.color = j.value("color", std::uint32_t{0xFF4F46E5u});
            h.daily = j.value("daily", true);
            h.weekdayMask = static_cast<std::uint8_t>(j.value("weekdayMask", 0x7F));
            h.createdAtMs = j.value("createdAtMs", EpochMs{0});
            if (h.name.empty()) return err("name is required").dump();
            return json{{"ok", true}, {"habit", habitToJson(mStore.createHabit(std::move(h)))}}.dump();
        }

        if (op == "habit.update") {
            const auto existing = mStore.habit(j.value("id", HabitId{0}));
            if (!existing) return err("habit not found").dump();
            Habit h = *existing;
            if (j.contains("name")) h.name = j["name"].get<std::string>();
            if (j.contains("description")) h.description = j["description"].get<std::string>();
            if (j.contains("icon")) h.icon = j["icon"].get<std::string>();
            if (j.contains("color")) h.color = j["color"].get<std::uint32_t>();
            if (j.contains("daily")) h.daily = j["daily"].get<bool>();
            if (j.contains("weekdayMask")) h.weekdayMask = static_cast<std::uint8_t>(j["weekdayMask"].get<unsigned>());
            if (j.contains("archived")) h.archived = j["archived"].get<bool>();
            mStore.updateHabit(h);
            return json{{"ok", true}, {"habit", habitToJson(h)}}.dump();
        }

        if (op == "habit.archive") {
            const bool ok = mStore.setHabitArchived(j.value("id", HabitId{0}),
                                                    j.value("archived", true));
            if (!ok) return err("habit not found").dump();
            return okJson().dump();
        }

        if (op == "habit.setCompleted") {
            const bool ok = mStore.setCompleted(j.value("id", HabitId{0}),
                                                j.value("day", DayNumber{0}),
                                                j.value("done", true));
            if (!ok) return err("habit not found").dump();
            return okJson().dump();
        }

        if (op == "habit.stats") {
            const HabitId id = j.value("id", HabitId{0});
            if (!mStore.habit(id)) return err("habit not found").dump();
            const DayNumber today = j.value("today", DayNumber{0});
            return json{{"ok", true},
                        {"currentStreak", mStore.currentStreak(id, today)},
                        {"longestStreak", mStore.longestStreak(id)},
                        {"doneToday", mStore.isCompleted(id, today)}}.dump();
        }

        // ------------------------------------------------------- Dziennik
        if (op == "journal.list") {
            auto arr = json::array();
            for (const auto& e : mStore.entries()) arr.push_back(entryToJson(e));
            return json{{"ok", true}, {"entries", std::move(arr)}}.dump();
        }

        if (op == "journal.get") {
            const auto e = mStore.entry(j.value("id", EntryId{0}));
            if (!e) return err("entry not found").dump();
            return json{{"ok", true}, {"entry", entryToJson(*e)}}.dump();
        }

        if (op == "journal.create") {
            JournalEntry e;
            e.title = j.value("title", std::string{});
            e.body = j.value("body", std::string{});
            e.mood = static_cast<Mood>(j.value("mood", 3));
            e.createdAtMs = j.value("createdAtMs", EpochMs{0});
            e.editedAtMs = j.value("editedAtMs", EpochMs{0});
            const EntryId id = mStore.createEntry(std::move(e));
            return json{{"ok", true}, {"entry", entryToJson(*mStore.entry(id))}}.dump();
        }

        if (op == "journal.update") {
            const auto existing = mStore.entry(j.value("id", EntryId{0}));
            if (!existing) return err("entry not found").dump();
            JournalEntry e = *existing;
            if (j.contains("title")) e.title = j["title"].get<std::string>();
            if (j.contains("body")) e.body = j["body"].get<std::string>();
            if (j.contains("mood")) e.mood = static_cast<Mood>(j["mood"].get<int>());
            if (j.contains("editedAtMs")) e.editedAtMs = j["editedAtMs"].get<EpochMs>();
            mStore.updateEntry(e);
            return json{{"ok", true}, {"entry", entryToJson(*mStore.entry(e.id))}}.dump();
        }

        if (op == "journal.remove") {
            if (!mStore.removeEntry(j.value("id", EntryId{0}))) return err("entry not found").dump();
            return okJson().dump();
        }

        // ------------------------------------------------------ Nauka (SRS)
        if (op == "card.list") {
            auto arr = json::array();
            for (const auto& c : mStore.cards()) arr.push_back(cardToJson(c));
            return json{{"ok", true}, {"cards", std::move(arr)}}.dump();
        }

        if (op == "card.get") {
            const auto c = mStore.card(j.value("id", CardId{0}));
            if (!c) return err("card not found").dump();
            return json{{"ok", true}, {"card", cardToJson(*c)}}.dump();
        }

        if (op == "card.create") {
            Card c;
            c.deck = j.value("deck", std::string{});
            c.front = j.value("front", std::string{});
            c.back = j.value("back", std::string{});
            if (c.front.empty() || c.back.empty()) return err("front and back are required").dump();
            const CardId id = mStore.createCard(std::move(c));
            return json{{"ok", true}, {"card", cardToJson(*mStore.card(id))}}.dump();
        }

        if (op == "card.update") {
            const auto existing = mStore.card(j.value("id", CardId{0}));
            if (!existing) return err("card not found").dump();
            Card c = *existing;
            if (j.contains("deck")) c.deck = j["deck"].get<std::string>();
            if (j.contains("front")) c.front = j["front"].get<std::string>();
            if (j.contains("back")) c.back = j["back"].get<std::string>();
            mStore.updateCard(c);
            return json{{"ok", true}, {"card", cardToJson(*mStore.card(c.id))}}.dump();
        }

        if (op == "card.remove") {
            if (!mStore.removeCard(j.value("id", CardId{0}))) return err("card not found").dump();
            return okJson().dump();
        }

        if (op == "card.review") {
            const CardId id = j.value("id", CardId{0});
            const int quality = j.value("quality", 0);
            const EpochMs nowMs = j.value("nowMs", EpochMs{0});
            const auto updated = mStore.review(id, quality, nowMs);
            if (!updated) return err("card not found").dump();
            return json{{"ok", true}, {"card", cardToJson(*updated)}}.dump();
        }

        if (op == "card.due") {
            const EpochMs nowMs = j.value("nowMs", EpochMs{0});
            const std::size_t limit = j.value("limit", std::size_t{50});
            auto arr = json::array();
            for (const auto& c : mStore.dueCards(nowMs, limit)) arr.push_back(cardToJson(c));
            return json{{"ok", true}, {"cards", std::move(arr)}}.dump();
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
