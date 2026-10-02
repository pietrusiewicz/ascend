# ADR 0001 — Granice rdzenia (core)

Data: 2026-10-01 · Status: przyjęty (zakres: pomocnik do medytacji)

## Kontekst

Aplikacja ma mieć całą logikę domenową w natywnym C++, a warstwa Android ma
być wymienna. Żeby rdzeń dało się testować na hoście i przenosić między
platformami, musi być odcięty od Androida.

## Decyzja

1. **`core/` nie zawiera żadnego `#include <jni.h>` ani `<android/...>`.**
   Jedyną warstwą znającą Androida jest `bridge/`.
2. **Rdzeń nie czyta dysku ani sieci.** Dane wejściowe (np. snapshot JSON)
   dostarcza warstwa platformowa jako bufor bajtów; trwałość to `json_io`
   (serializacja) + plik zapisywany po stronie Kotlina.
3. **Cały stan domenowy przechodzi przez `Store`** (sesje + wzorce oddechu).
   Fasada (`Facade`) jest cienka i deterministyczna; udostępnia kanał poleceń JSON.
4. **Czas jest zawsze `EpochMs` (ms epoki Unix, UTC).** „Dni" (passa medytacji)
   liczone są jako `dayNumberUtc(ms)` — numer dnia UTC.
5. **Statystyki są liczone na żądanie** z listy sesji (`computeStats`), a nie
   przechowywane — dzięki temu nie da się ich rozjechać ze stanem.

## Konsekwencje

- Testy jednostkowe uruchamiają się zwykłym CMake na hoście (bez urządzenia).
- Jeden format zapisu (JSON v2) działa na każdej platformie.
- Mostek JNI to wyłącznie konwersja typów — bez logiki domenowej.
