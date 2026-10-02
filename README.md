# Ascend

Aplikacja mobilna do samorozwoju w **natywnym C++** (Android NDK + JNI).
Cała logika domenowa mieszka w przenośnym rdzeniu C++23; Android jest tylko
cienką nakładką UI (Kotlin + Jetpack Compose).

## Funkcje (MVP „wszystko po trochu")

- **Nawyki** — codzienne/tygodniowe nawyki, odhaczanie, licznik serii (streak).
- **Dziennik** — wpisy z nastrojem (1–5), dodawanie i usuwanie.
- **Nauka** — fiszki z powtórkami rozłożonymi w czasie (algorytm SM-2).
- **i18n** — polski i angielski od początku (Android `values` + `values-pl`).
- Cały stan w jednym snapshotcie JSON (wersjonowany, zapis lokalny).

## Architektura

```
core/      przenośny rdzeń C++23 (ZERO zależności od Androida)
  ├── types.h, habit.h, journal.h, srs.h, store.h, facade.h, json_io.h
bridge/    mostek JNI (jedyna warstwa znająca Androida)
android/   Gradle + Kotlin + Compose (UI, i18n, trwałość snapshotu)
```

- `ascend_core` — biblioteka statyczna (logika domenowa + JSON).
- `ascend_bridge` — `libascend_bridge.so` (kanał poleceń JSON przez JNI).
- `pl.ascend.engine.NativeBridge` — punkt wejścia z Kotlina.

Szczegóły: [`docs/ADR/0001-core-boundaries.md`](docs/ADR/0001-core-boundaries.md)
i [`docs/PROTOCOL.md`](docs/PROTOCOL.md).

## Budowanie

Wymagania: CMake ≥ 3.22, kompilator C++23 (host), Android SDK 35 + NDK 27.2.

```bash
# Testy rdzenia na hoście (szybka pętla developerska)
cmake --preset host-debug
cmake --build --preset host-debug -j
ctest --preset host-debug --output-on-failure

# APK (debug)
cd android
./gradlew :app:assembleDebug
```

Pełne instrukcje: [`docs/BUILD.md`](docs/BUILD.md).

## Wystawianie w sklepie

Plik APK publikuje się w sklepie z aplikacjami serwisu Beginner3D jako wydanie
`ascend-<wersja>-android-arm64` (z towarzyszącym `.sha256`) w katalogu
`BEGINNER3D_AGENT_ROOT`, a ikonę `ascend.png` w `BEGINNER3D_APLIKACJE_ROOT`.
Wpis w sklepie tworzy polecenie `opublikuj_aplikacje_ascend`.

## Licencja

GPL-3.0-or-later — patrz [LICENSE](LICENSE).
