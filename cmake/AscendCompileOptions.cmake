# Wspólne flagi kompilacji rdzenia i mostka JNI.
#
# Zasady:
#  * C++23 bez rozszerzeń GNU — kod buduje się identycznie na GCC (host),
#    clang z NDK (urządzenie) i w CI (x86_64).
#  * Wyjątki i RTTI włączone (uproszczenie kodu; hot-path bez wyjątków).
#  * 16 KB page size dla .so (wymóg Androida 15+ / Google Play).
#
# SPDX-License-Identifier: GPL-3.0-or-later

include_guard(GLOBAL)

function(ascend_apply_compile_options)
    add_compile_options(-Wall -Wextra -Wpedantic
                        -Wshadow -Wnon-virtual-dtor -Wold-style-cast
                        -Wcast-qual -Wconversion -Wsign-conversion
                        -Wnull-dereference -Wdouble-promotion
                        -Wimplicit-fallthrough -Wuseless-cast
                        -Wformat=2)
    # Rdzeń jest wkompilowany w libascend_bridge.so (NDK) → wymagany -fPIC.
    add_compile_options(-fPIC)

    if(ASC_WERROR)
        add_compile_options(-Werror)
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-fno-strict-aliasing)
        add_compile_options("$<$<CONFIG:Release>:-O2>")
        add_compile_options("$<$<CONFIG:Release>:-fno-plt>")
        add_compile_options("$<$<CONFIG:RelWithDebInfo>:-O2;-g>")
        add_compile_options("$<$<CONFIG:Release>:-fvisibility=hidden;-fvisibility-inlines-hidden>")
    endif()

    if(ASC_SANITIZERS)
        if(ANDROID)
            message(FATAL_ERROR "ASC_SANITIZERS jest przewidziane tylko dla buildu hosta.")
        endif()
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -g)
        add_link_options(-fsanitize=address,undefined)
    endif()

    # 16 KB alignment dla .so — wymagane przez Google Play, zalecane dla F-Droid.
    if(ANDROID)
        add_link_options("-Wl,-z,max-page-size=16384")
    endif()
endfunction()
