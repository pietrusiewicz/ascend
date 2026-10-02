// Mostek do natywnej biblioteki libascend_bridge.so (C++).
// SPDX-License-Identifier: GPL-3.0-or-later
package pl.ascend.engine

object NativeBridge {
    init {
        System.loadLibrary("ascend_bridge")
    }

    external fun nativeCommand(cmd: String): String
    external fun nativeSnapshot(): String
    external fun nativeLoadSnapshot(json: String): Boolean
    external fun nativeProtocolVersion(): Int
    external fun nativeSaveSchemaVersion(): Int
    external fun nativeCoreVersion(): String
}
