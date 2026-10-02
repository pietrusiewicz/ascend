# Ascend — reguły ProGuard/R8.
# Metody JNI muszą pozostać niezmienione (nazwy są wiązane w C++).
-keepclasseswithmembernames class pl.ascend.engine.NativeBridge {
    native <methods>;
}
