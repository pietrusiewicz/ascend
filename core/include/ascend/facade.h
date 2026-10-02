// Ascend — publiczna fasada rdzenia.
//
// Udostępnia: kanał poleceń JSON (używany przez mostek JNI), trwałość
// (zrzut/wczytanie stanu) oraz metadane wersji. Cała logika domenowa
// mieszka w Store + modułach; fasada jest cienka i deterministyczna.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include "ascend/store.h"

namespace ascend {

class Facade {
public:
    // Kanał sterujący. Każde polecenie to obiekt JSON z polem "op".
    // Odpowiedź to zawsze obiekt JSON z polem "ok" (true/false).
    // Wyrzuca std::exception tylko przy nieprawidłowym JSON wejściowym.
    std::string command(std::string_view json);

    // Zrzut/wczytanie pełnego stanu (format zapisu v1).
    std::string snapshotToJson() const;
    bool loadSnapshot(std::string_view raw, std::string& err);

    static std::int32_t protocolVersion();
    static std::int32_t saveSchemaVersion();
    static std::string coreVersion();

private:
    Store mStore;
};

}  // namespace ascend
