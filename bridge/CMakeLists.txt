# Mostek JNI — JEDYNA warstwa projektu, która wie o istnieniu Androida/JNI.
#
# Zasady (zob. docs/ADR/0001-core-boundaries.md):
#   * zero logiki domenowej — tylko konwersja typów JNI <-> C++ i zarządzanie pamięcią,
#   * rdzeń (ascend_core) dołączany jako podkatalog, jeśli jeszcze nie zdefiniowany,
#     dzięki czemu Gradle może wskazać externalNativeBuild bezpośrednio na ten plik.
#
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.22)

project(ascend_bridge VERSION 0.1.0 LANGUAGES CXX)

if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

# --- Dołączenie rdzenia i modułów cmake, gdy ten plik jest korzeniem buildu ---
if(NOT TARGET ascend_core)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")
    include(AscendCompileOptions)
    ascend_apply_compile_options()
    include(AscendDependencies)
    # Testy hosta i sam bridge nie są tu potrzebne, gdy buduje nas Gradle.
    set(ASC_BUILD_TESTS OFF CACHE INTERNAL "")
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../core" "${CMAKE_BINARY_DIR}/ascend_core")
endif()

add_library(ascend_bridge SHARED
        src/jni_facade.cpp
        src/jni_util.cpp)

target_include_directories(ascend_bridge PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(ascend_bridge PRIVATE ascend::core)

if(ANDROID)
    find_library(log-lib log)
    find_library(android-lib android)
    # Jawne linkowanie libc++ (c++_static zgodnie z -DANDROID_STL=c++_static).
    target_link_libraries(ascend_bridge PRIVATE ${log-lib} ${android-lib} c++_static c++abi)
    target_compile_definitions(ascend_bridge PRIVATE ASC_PLATFORM_ANDROID=1)
else()
    # Build mostka poza Androidem (sprawdzenie kompilacji) — wymaga jni.h z JDK.
    if(DEFINED ENV{JAVA_HOME})
        target_include_directories(ascend_bridge PRIVATE
                "$ENV{JAVA_HOME}/include"
                "$ENV{JAVA_HOME}/include/linux")
    endif()
    message(WARNING
            "ascend_bridge budowany poza Androidem — potrzebne jni.h (ustaw JAVA_HOME). "
            "Ten wariant służy tylko do sprawdzania kompilacji, nie do uruchamiania.")
endif()

set_target_properties(ascend_bridge PROPERTIES
        OUTPUT_NAME "ascend_bridge"
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON)

# Eksportujemy wyłącznie symbole JNI (Java_...); reszta rdzenia pozostaje ukryta.
if(NOT ANDROID AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_link_options(ascend_bridge PRIVATE "-Wl,--exclude-libs,ALL")
endif()
