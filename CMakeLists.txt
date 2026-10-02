# Ascend — przenośny rdzeń C++23.
#
# Ten plik jest punktem wejścia do budowy NA HOŚCIE (testy jednostkowe).
# Na Androidzie Gradle wywołuje bridge/CMakeLists.txt (zob. android/app/build.gradle.kts),
# który dołącza core/ jako podkatalog — logika budowy rdzenia jest więc wspólna.
#
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.22)

project(ascend
        VERSION 0.2.0
        DESCRIPTION "Ascend — meditation helper core (sessions, breathing, stats)"
        LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Opcje
# ---------------------------------------------------------------------------
option(ASC_BUILD_TESTS  "Buduj testy jednostkowe rdzenia (GoogleTest, tylko host)" ON)
option(ASC_BUILD_BRIDGE "Buduj mostek JNI (wymaga nagłówków jni.h — Android/NDK)"   OFF)
option(ASC_WERROR       "Traktuj ostrzeżenia kompilatora jako błędy"               OFF)
option(ASC_SANITIZERS   "Włącz ASan+UBSan (tylko host, debug)"                     OFF)

set(ASC_DEPS_DIR "" CACHE PATH
    "Katalog z wcześniej pobranymi zależnościami (googletest/, json/); pusty = FetchContent")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# ---------------------------------------------------------------------------
# Ustawienia globalne
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(AscendCompileOptions)
ascend_apply_compile_options()

include(AscendDependencies)

# ---------------------------------------------------------------------------
# Komponenty
# ---------------------------------------------------------------------------
add_subdirectory(core)

if(ASC_BUILD_BRIDGE)
    add_subdirectory(bridge)
endif()

if(ASC_BUILD_TESTS)
    enable_testing()
    add_subdirectory(core/tests)
endif()

# ---------------------------------------------------------------------------
# Podsumowanie konfiguracji
# ---------------------------------------------------------------------------
message(STATUS "-------------------------------------------------------")
message(STATUS " ${PROJECT_NAME} ${PROJECT_VERSION}  (${CMAKE_BUILD_TYPE})")
message(STATUS "   testy        : ${ASC_BUILD_TESTS}")
message(STATUS "   mostek JNI   : ${ASC_BUILD_BRIDGE}")
message(STATUS "   sanitizery   : ${ASC_SANITIZERS}")
message(STATUS "   kompilator   : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "-------------------------------------------------------")
