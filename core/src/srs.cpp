// Ascend — implementacja algorytmu SM-2 (powtórki rozłożone w czasie).
// SPDX-License-Identifier: GPL-3.0-or-later
#include "ascend/srs.h"

#include <algorithm>
#include <cmath>

namespace ascend {

namespace {
constexpr double kMinEase = 1.3;
constexpr int kMinQuality = 0;
constexpr int kMaxQuality = 5;
}  // namespace

Card sm2Review(const Card& c, int quality, EpochMs nowMs) {
    Card out = c;
    const int q = std::clamp(quality, kMinQuality, kMaxQuality);

    if (q >= 3) {
        // Poprawna odpowiedź — wydłużamy interwał.
        if (out.repetitions == 0) {
            out.intervalDays = 1.0;
        } else if (out.repetitions == 1) {
            out.intervalDays = 6.0;
        } else {
            out.intervalDays = std::round(out.intervalDays * out.ease);
        }
        out.repetitions += 1;
    } else {
        // Zapomnienie — wracamy do powtórki „jutro" i liczymy lapse.
        out.repetitions = 0;
        out.lapses += 1;
        out.intervalDays = 1.0;
    }

    // Aktualizacja współczynnika łatwości (standardowy wzorzec SM-2).
    const double delta = static_cast<double>(kMaxQuality - q);
    out.ease = out.ease + (0.1 - delta * (0.08 + delta * 0.02));
    if (out.ease < kMinEase) out.ease = kMinEase;

    const auto intervalMs =
        static_cast<EpochMs>(std::llround(out.intervalDays * kMillisPerDay));
    out.dueMs = nowMs + intervalMs;
    return out;
}

}  // namespace ascend
