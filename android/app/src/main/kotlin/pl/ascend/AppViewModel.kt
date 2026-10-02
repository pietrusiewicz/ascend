// ViewModel: stan UI + wywołania repozytorium.
// SPDX-License-Identifier: GPL-3.0-or-later
package pl.ascend

import android.app.Application
import androidx.lifecycle.AndroidViewModel
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import pl.ascend.data.AscendRepository

data class SessionItem(
    val id: Long,
    val startedAtMs: Long,
    val durationSeconds: Long,
    val technique: String,
    val patternId: Long,
    val moodBefore: Int?,
    val moodAfter: Int?,
    val notes: String,
)

data class PatternItem(
    val id: Long,
    val name: String,
    val inhaleSeconds: Int,
    val holdInSeconds: Int,
    val exhaleSeconds: Int,
    val holdOutSeconds: Int,
    val builtin: Boolean,
) {
    val cycleSeconds: Int
        get() = inhaleSeconds + holdInSeconds + exhaleSeconds + holdOutSeconds
}

data class Stats(
    val streakDays: Long,
    val longestStreak: Long,
    val totalSessions: Long,
    val totalSeconds: Long,
    val sessionsToday: Long,
    val secondsToday: Long,
)

class AppViewModel(app: Application) : AndroidViewModel(app) {

    private val repo = AscendRepository(app)

    // Numer dnia UTC (spójny z rdzeniem C++: dayNumberUtc).
    private val today: Long
        get() = System.currentTimeMillis() / 86_400_000L

    private val _sessions = MutableStateFlow<List<SessionItem>>(emptyList())
    val sessions: StateFlow<List<SessionItem>> = _sessions

    private val _patterns = MutableStateFlow<List<PatternItem>>(emptyList())
    val patterns: StateFlow<List<PatternItem>> = _patterns

    private val _stats = MutableStateFlow(Stats(0, 0, 0, 0, 0, 0))
    val stats: StateFlow<Stats> = _stats

    init {
        repo.load()
        refreshAll()
    }

    fun refreshAll() {
        refreshSessions()
        refreshPatterns()
        refreshStats()
    }

    // ----- Sesje ---------------------------------------------------------

    fun refreshSessions() {
        val arr = repo.listSessions()
        val items = ArrayList<SessionItem>(arr.length())
        for (i in 0 until arr.length()) {
            val s = arr.getJSONObject(i)
            items += SessionItem(
                id = s.getLong("id"),
                startedAtMs = s.optLong("startedAtMs"),
                durationSeconds = s.optLong("durationSeconds"),
                technique = s.optString("technique", "breath"),
                patternId = s.optLong("patternId", 0),
                moodBefore = if (s.has("moodBefore")) s.optInt("moodBefore") else null,
                moodAfter = if (s.has("moodAfter")) s.optInt("moodAfter") else null,
                notes = s.optString("notes"),
            )
        }
        _sessions.value = items
    }

    fun saveSession(
        startedAtMs: Long,
        durationSeconds: Long,
        technique: String,
        patternId: Long,
        moodBefore: Int?,
        moodAfter: Int?,
        notes: String,
    ) {
        repo.createSession(startedAtMs, durationSeconds, technique, patternId, moodBefore, moodAfter, notes)
        refreshSessions()
        refreshStats()
    }

    fun removeSession(id: Long) {
        repo.removeSession(id)
        refreshSessions()
        refreshStats()
    }

    // ----- Wzorce oddechu -------------------------------------------------

    fun refreshPatterns() {
        val arr = repo.listPatterns()
        val items = ArrayList<PatternItem>(arr.length())
        for (i in 0 until arr.length()) {
            val p = arr.getJSONObject(i)
            items += PatternItem(
                id = p.getLong("id"),
                name = p.optString("name"),
                inhaleSeconds = p.optInt("inhaleSeconds", 4),
                holdInSeconds = p.optInt("holdInSeconds", 0),
                exhaleSeconds = p.optInt("exhaleSeconds", 4),
                holdOutSeconds = p.optInt("holdOutSeconds", 0),
                builtin = p.optBoolean("builtin"),
            )
        }
        _patterns.value = items
    }

    // ----- Statystyki ------------------------------------------------------

    fun refreshStats() {
        val s = repo.stats(today)
        _stats.value = Stats(
            streakDays = s.optLong("streakDays"),
            longestStreak = s.optLong("longestStreak"),
            totalSessions = s.optLong("totalSessions"),
            totalSeconds = s.optLong("totalSeconds"),
            sessionsToday = s.optLong("sessionsToday"),
            secondsToday = s.optLong("secondsToday"),
        )
    }
}
