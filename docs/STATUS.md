# Status

## Gotowe

- Przenośny rdzeń C++23: sesje medytacji, wzorce oddechu, statystyki (passa),
  magazyn, JSON, fasada z kanałem poleceń.
- Mostek JNI (`libascend_bridge.so`) — kanał poleceń JSON + snapshot.
- Testy jednostkowe hosta (28 testów, GoogleTest).
- Warstwa Android: Kotlin + Compose, 3 ekrany (Medytuj / Dziennik / Statystyki),
  prowadzenie oddechu z animacją, i18n PL/EN.
- Build APK (debug/release) przez Gradle; wydanie podpisane kluczem release.

## W trakcie / plan

- Zrzuty ekranu do sklepu (opcjonalnie).
- Dzwonek na koniec sesji (obecnie tylko timer; dźwięk w warstwie platformowej).
- Auto-aktualizacja wersji / historia wydań.
