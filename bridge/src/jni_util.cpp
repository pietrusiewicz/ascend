// Mostek JNI — implementacja pomocników RAII.
// SPDX-License-Identifier: GPL-3.0-or-later
#include "ascend_bridge/jni_env.h"

#include <memory>
#include <string>

#include "ascend/facade.h"

namespace ascend::jni {

namespace {
// Singleton Facade przechowywany przez lifetime procesu JVM (loadLibrary).
std::unique_ptr<ascend::Facade> gFacade;
}  // namespace

Facade& facade() {
    if (!gFacade) gFacade = std::make_unique<ascend::Facade>();
    return *gFacade;
}

void throwRuntimeError(JNIEnv* env, std::string_view message) {
    if (env == nullptr) return;
    if (env->ExceptionCheck()) {
        // Wyjątek już w toku — nie nadpisujemy.
        return;
    }
    jclass cls = env->FindClass("java/lang/RuntimeException");
    if (cls == nullptr) {
        env->ExceptionClear();
        return;
    }
    env->ThrowNew(cls, std::string(message).c_str());
    env->DeleteLocalRef(cls);
}

std::string toUtf8(JNIEnv* env, jstring jstr) {
    if (jstr == nullptr) return {};
    const char* raw = env->GetStringUTFChars(jstr, nullptr);
    if (raw == nullptr) return {};
    std::string out(raw);
    env->ReleaseStringUTFChars(jstr, raw);
    return out;
}

jstring fromUtf8(JNIEnv* env, std::string_view text) {
    if (env == nullptr) return nullptr;
    return env->NewStringUTF(std::string(text).c_str());
}

}  // namespace ascend::jni
