# Testy jednostkowe rdzenia — uruchamiane NA HOŚCIE (g++/clang, bez Androida).
# To najszybsza pętla developerska i jedyne realne testy w środowisku
# bez urządzenia i emulatora.
#
# SPDX-License-Identifier: GPL-3.0-or-later

ascend_add_googletest()

add_executable(ascend_core_tests
        test_main.cpp
        test_habit.cpp
        test_journal.cpp
        test_srs.cpp
        test_store.cpp
        test_json_io.cpp
        test_facade.cpp)

target_link_libraries(ascend_core_tests PRIVATE
        ascend::core
        GTest::gtest
        GTest::gtest_main)

# Testy nie są biblioteką produktową — nie ukrywajmy symboli (łatwiejszy debug).
set_target_properties(ascend_core_tests PROPERTIES CXX_VISIBILITY_PRESET default)

include(GoogleTest)
gtest_discover_tests(ascend_core_tests
        PROPERTIES TIMEOUT 60
        DISCOVERY_TIMEOUT 60)
