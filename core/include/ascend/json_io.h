// Ascend — serializacja do/z JSON (format zapisu v1).
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <string>
#include <string_view>

#include <nlohmann/json.hpp>

#include "ascend/store.h"

namespace ascend {

// Konwertery pojedynczych encji (współdzielone przez snapshot i kanał poleceń).
nlohmann::json habitToJson(const Habit& h);
nlohmann::json entryToJson(const JournalEntry& e);
nlohmann::json cardToJson(const Card& c);

// Zwraca pełny zrzut stanu jako JSON ({"schema":1, ...}).
std::string storeToJson(const Store& store);

// Wczytuje stan z JSON. Przy sukcesie zwraca true i nadpisuje `store`;
// przy błędzie zwraca false i zapisuje czytelny komunikat do `err`.
bool storeFromJson(std::string_view json, Store& store, std::string& err);

}  // namespace ascend
