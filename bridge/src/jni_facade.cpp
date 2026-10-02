// Mostek JNI — punkty wejścia dla pl.ascend.engine.NativeBridge.
//
// Każdy wyjątek C++ jest łapany i zamieniany na RuntimeException;
// błędy domenowe wracają jako JSON {"ok":false,...} przez kanał poleceń.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include <jni.h>

#include <cstdint>
#include <memory>
#include <new>
#include <string>

#include "ascend/facade.h"
#include "ascend_bridge/jni_env.h"

extern "C" {

// ----- Kanał poleceń ---------------------------------------------------

JNIEXPORT jstring JNICALL
Java_pl_ascend_engine_NativeBridge_nativeCommand(JNIEnv* env, jclass,
                                                 jstring jCmd) {
    const std::string cmd = ascend::jni::toUtf8(env, jCmd);
    if (env->ExceptionCheck()) return nullptr;
    std::string out;
    try {
        out = ascend::jni::facade().command(cmd);
    } catch (const std::bad_alloc&) {
        ascend::jni::throwRuntimeError(env, "ascend: bad_alloc during command()");
        return nullptr;
    } catch (const std::exception& e) {
        ascend::jni::throwRuntimeError(env, std::string("ascend: ") + e.what());
        return nullptr;
    } catch (...) {
        ascend::jni::throwRuntimeError(env, "ascend: unknown native exception");
        return nullptr;
    }
    return ascend::jni::fromUtf8(env, out);
}

// ----- Save / load (snapshot JSON) -------------------------------------

JNIEXPORT jstring JNICALL
Java_pl_ascend_engine_NativeBridge_nativeSnapshot(JNIEnv* env, jclass) {
    std::string out;
    try {
        out = ascend::jni::facade().snapshotToJson();
    } catch (const std::exception& e) {
        ascend::jni::throwRuntimeError(env, std::string("ascend: ") + e.what());
        return nullptr;
    }
    return ascend::jni::fromUtf8(env, out);
}

JNIEXPORT jboolean JNICALL
Java_pl_ascend_engine_NativeBridge_nativeLoadSnapshot(JNIEnv* env, jclass,
                                                      jstring jSnapshot) {
    const std::string snapshot = ascend::jni::toUtf8(env, jSnapshot);
    if (env->ExceptionCheck()) return JNI_FALSE;
    std::string err;
    bool ok = false;
    try {
        ok = ascend::jni::facade().loadSnapshot(snapshot, err);
    } catch (const std::exception& e) {
        ascend::jni::throwRuntimeError(env, std::string("ascend: ") + e.what());
        return JNI_FALSE;
    }
    if (!ok && !err.empty()) {
        ascend::jni::throwRuntimeError(env, err);
        return JNI_FALSE;
    }
    return ok ? JNI_TRUE : JNI_FALSE;
}

// ----- Metainformacje ---------------------------------------------------

JNIEXPORT jint JNICALL
Java_pl_ascend_engine_NativeBridge_nativeProtocolVersion(JNIEnv*, jclass) {
    return ascend::Facade::protocolVersion();
}

JNIEXPORT jint JNICALL
Java_pl_ascend_engine_NativeBridge_nativeSaveSchemaVersion(JNIEnv*, jclass) {
    return ascend::Facade::saveSchemaVersion();
}

JNIEXPORT jstring JNICALL
Java_pl_ascend_engine_NativeBridge_nativeCoreVersion(JNIEnv* env, jclass) {
    return ascend::jni::fromUtf8(env, ascend::Facade::coreVersion());
}

}  // extern "C"
