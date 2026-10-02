// Mostek JNI — pomocnicze RAII do obsługi JNIEnv bez wycieków referencji.
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <jni.h>

#include <cstddef>
#include <string>
#include <string_view>

#include "ascend/facade.h"

namespace ascend::jni {

// Globalny singleton Facade — jeden rdzeń na proces JVM.
Facade& facade();

// Podnosi java.lang.RuntimeException z komunikatem. Bezpieczne przy wielokrotnym
// wywołaniu — pomija, gdy wyjątek jest już w toku.
void throwRuntimeError(JNIEnv* env, std::string_view message);

// Zwraca ciąg UTF-8 z java.lang.String (pusty dla nullptr/błędu).
std::string toUtf8(JNIEnv* env, jstring jstr);

// Tworzy java.lang.String z bufora UTF-8 (nullptr przy błędzie alokacji).
jstring fromUtf8(JNIEnv* env, std::string_view text);

}  // namespace ascend::jni
