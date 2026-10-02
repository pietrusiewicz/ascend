// ViewModel: stan UI + wywołania repozytorium.
// SPDX-License-Identifier: GPL-3.0-or-later
package pl.ascend

import android.app.Application
import androidx.lifecycle.AndroidViewModel
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import pl.ascend.data.AscendRepository

data class HabitItem(
    val id: Long,
    val name: String,
    val description: String,
    val currentStreak: Long,
    val longestStreak: Long,
    val doneToday: Boolean,
)

data class EntryItem(
    val id: Long,
    val title: String,
    val body: String,
    val mood: Int,
    val createdAtMs: Long,
)

data class CardItem(
    val id: Long,
    val deck: String,
    val front: String,
    val back: String,
)

data class Overview(
    val activeHabits: Int,
    val doneToday: Int,
    val dueCards: Int,
    val entries: Int,
)

class AppViewModel(app: Application) : AndroidViewModel(app) {

    private val repo = AscendRepository(app)

    // Numer dnia UTC (spójny z rdzeniem C++: dayNumberUtc).
    private val today: Long
        get() = System.currentTimeMillis() / 86_400_000L

    private val _habits = MutableStateFlow<List<HabitItem>>(emptyList())
    val habits: StateFlow<List<HabitItem>> = _habits

    private val _entries = MutableStateFlow<List<EntryItem>>(emptyList())
    val entries: StateFlow<List<EntryItem>> = _entries

    private val _dueCards = MutableStateFlow<List<CardItem>>(emptyList())
    val dueCards: StateFlow<List<CardItem>> = _dueCards

    private val _overview = MutableStateFlow(Overview(0, 0, 0, 0))
    val overview: StateFlow<Overview> = _overview

    init {
        repo.load()
        refreshAll()
    }

    fun refreshAll() {
        refreshHabits()
        refreshEntries()
        refreshCards()
        refreshOverview()
    }

    // ----- Nawyki ---------------------------------------------------------

    fun refreshHabits() {
        val arr = repo.listHabits()
        val items = ArrayList<HabitItem>(arr.length())
        for (i in 0 until arr.length()) {
            val h = arr.getJSONObject(i)
            val id = h.getLong("id")
            val stats = repo.habitStats(id, today)
            items += HabitItem(
                id = id,
                name = h.optString("name"),
                description = h.optString("description"),
                currentStreak = stats.optLong("currentStreak"),
                longestStreak = stats.optLong("longestStreak"),
                doneToday = stats.optBoolean("doneToday"),
            )
        }
        _habits.value = items
    }

    fun toggleHabit(id: Long) {
        val habit = _habits.value.firstOrNull { it.id == id } ?: return
        repo.setCompleted(id, today, !habit.doneToday)
        refreshHabits()
        refreshOverview()
    }

    fun addHabit(name: String, description: String) {
        repo.createHabit(name, description)
        refreshHabits()
        refreshOverview()
    }

    // ----- Dziennik --------------------------------------------------------

    fun refreshEntries() {
        val arr = repo.listEntries()
        val items = ArrayList<EntryItem>(arr.length())
        for (i in 0 until arr.length()) {
            val e = arr.getJSONObject(i)
            items += EntryItem(
                id = e.getLong("id"),
                title = e.optString("title"),
                body = e.optString("body"),
                mood = e.optInt("mood"),
                createdAtMs = e.optLong("createdAtMs"),
            )
        }
        _entries.value = items
    }

    fun addEntry(title: String, body: String, mood: Int) {
        repo.createEntry(title, body, mood, System.currentTimeMillis())
        refreshEntries()
        refreshOverview()
    }

    fun removeEntry(id: Long) {
        repo.removeEntry(id)
        refreshEntries()
        refreshOverview()
    }

    // ----- Nauka (SRS) ------------------------------------------------------

    fun refreshCards() {
        val arr = repo.dueCards(System.currentTimeMillis(), 50)
        val items = ArrayList<CardItem>(arr.length())
        for (i in 0 until arr.length()) {
            val c = arr.getJSONObject(i)
            items += CardItem(
                id = c.getLong("id"),
                deck = c.optString("deck"),
                front = c.optString("front"),
                back = c.optString("back"),
            )
        }
        _dueCards.value = items
    }

    fun addCard(deck: String, front: String, back: String) {
        repo.createCard(deck, front, back)
        refreshCards()
    }

    fun reviewCard(id: Long, quality: Int) {
        repo.reviewCard(id, quality, System.currentTimeMillis())
        refreshCards()
        refreshOverview()
    }

    fun removeCard(id: Long) {
        repo.removeCard(id)
        refreshCards()
    }

    // ----- Podsumowanie -----------------------------------------------------

    fun refreshOverview() {
        val o = repo.overview(today)
        _overview.value = Overview(
            activeHabits = o.optInt("activeHabits"),
            doneToday = o.optInt("doneToday"),
            dueCards = o.optInt("dueCards"),
            entries = o.optInt("entries"),
        )
    }
}
