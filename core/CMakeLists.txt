# Rdzeń aplikacji: czysty C++23, ZERO zależności od Androida/JNI/platformy.
#
# Reguły nienaruszalne (zob. docs/ADR/0001-core-boundaries.md):
#   1. Żadnego #include <jni.h>, <android/...> ani API Androida w core/.
#   2. Rdzeń nie czyta z dysku ani z sieci — trwałość zapewnia warstwa platformowa.
#   3. Cały stan domenowy przechodzi przez Store; fasada jest deterministyczna.
#
# SPDX-License-Identifier: GPL-3.0-or-later

add_library(ascend_core STATIC
        src/srs.cpp
        src/store.cpp
        src/json_io.cpp
        src/facade.cpp)

add_library(ascend::core ALIAS ascend_core)

target_include_directories(ascend_core PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_compile_features(ascend_core PUBLIC cxx_std_23)

target_link_libraries(ascend_core PUBLIC nlohmann_json::nlohmann_json)

# Wersja rdzenia dostępna w kodzie (ekran „O aplikacji", logi).
target_compile_definitions(ascend_core PUBLIC
        ASC_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        ASC_VERSION_MINOR=${PROJECT_VERSION_MINOR}
        ASC_VERSION_PATCH=${PROJECT_VERSION_PATCH}
        ASC_VERSION_STRING="${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

set_target_properties(ascend_core PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON)
