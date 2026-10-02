// Ascend — serializacja do/z JSON (format zapisu v2).
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <string>
#include <string_view>

#include <nlohmann/json.hpp>

#include "ascend/store.h"

namespace ascend {

// Konwertery pojedynczych encji (współdzielone przez snapshot i kanał poleceń).
nlohmann::json sessionToJson(const Session& s);
nlohmann::json patternToJson(const BreathingPattern& p);

// Zamienia nazwę techniki na string snake_case ("breath", "silence", ...).
std::string techniqueToString(Technique t);
Technique techniqueFromString(std::string_view s);

// Zwraca pełny zrzut stanu jako JSON ({"schema":2, ...}).
std::string storeToJson(const Store& store);

// Wczytuje stan z JSON. Przy sukcesie zwraca true i nadpisuje `store`;
// przy błędzie zwraca false i zapisuje czytelny komunikat do `err`.
bool storeFromJson(std::string_view json, Store& store, std::string& err);

}  // namespace ascend
