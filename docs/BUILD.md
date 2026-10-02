# Budowanie

## Testy rdzenia (host)

Rdzeń jest niezależny od Androida, więc testy jednostkowe uruchamiasz
zwykłym CMake na hoście:

```bash
cmake --preset host-debug
cmake --build --preset host-debug -j
ctest --preset host-debug --output-on-failure
```

Dostępne presety (zob. `CMakePresets.json`): `host-debug`, `host-release`,
`host-asan` (ASan/UBSan), `host-werror` (ostrzeżenia jako błędy).

Zależności (`nlohmann/json`, `googletest`) pobiera `FetchContent`
z weryfikacją SHA256. Bez sieci możesz przygotować je lokalnie:

```bash
scripts/fetch-deps.sh /sciezka/do/deps   # tworzy json/ i googletest/
cmake --preset host-debug -DASC_DEPS_DIR=/sciezka/do/deps
```

## Weryfikacja mostka JNI (bez Androida)

Mostek kompiluje się też na hoście, żeby szybko wychwycić błędy `jni.h`
bez pełnego buildu Gradle:

```bash
export JAVA_HOME=/sciezka/do/jdk   # pełne JDK (z include/jni.h)
cmake -S . -B build/bridge-check -DASC_BUILD_BRIDGE=ON -DASC_BUILD_TESTS=OFF
cmake --build build/bridge-check -j
```

## APK (Android)

Wymagania: JDK 17+, Android SDK 35, NDK 27.2.12479018, CMake 3.22.1 (SDK).

```bash
cd android
echo "sdk.dir=/sciezka/do/android-sdk" > local.properties
./gradlew :app:assembleDebug          # debug (podpisany kluczem debug)
./gradlew :app:assembleRelease        # release (unsigned; podpisz osobno)
```

APK trafia do `android/app/build/outputs/apk/`.

### Uwaga do tej maszyny (aarch64 + NDK x86_64)

Na hoście aarch64 build działa dzięki dwóm rzeczom:

- `GRADLE_USER_HOME` wskazuje na cache z dystrybucją Gradle i zależnościami,
- prebuilt NDK `.../toolchains/llvm/prebuilt/linux-x86_64/bin/` jest owinięty
  skryptami box64 (prawdziwe binary w `bin.orig/`).

```bash
export GRADLE_USER_HOME=/opt/beginner3d-data/narzedzia/gradle-home
export ANDROID_HOME=/opt/beginner3d-data/narzedzia/android-sdk
export JAVA_HOME=/opt/beginner3d-data/narzedzia/jdk-21.0.12.1+1
```

Na zwykłym x86_64 te zmienne nie są potrzebne (standardowy NDK działa natywnie).
