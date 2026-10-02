// Repozytorium: cienka warstwa nad NativeBridge + trwałość snapshotu JSON.
// Cała logika domenowa mieszka w rdzeniu C++; tutaj tylko (de)serializacja JSON
// i zapis/odczyt pliku snapshotu.
// SPDX-License-Identifier: GPL-3.0-or-later
package pl.ascend.data

import android.content.Context
import org.json.JSONArray
import org.json.JSONObject
import pl.ascend.engine.NativeBridge
import java.io.File

class AscendRepository(context: Context) {

    private val saveFile = File(context.filesDir, "ascend.json")

    fun load() {
        if (saveFile.exists()) {
            runCatching { NativeBridge.nativeLoadSnapshot(saveFile.readText()) }
        }
    }

    // ----- kanał poleceń -------------------------------------------------

    private fun command(json: String): JSONObject = JSONObject(NativeBridge.nativeCommand(json))

    // Polecenia mutujące zapisują snapshot po sukcesie.
    private fun exec(json: String): JSONObject {
        val result = command(json)
        if (result.optBoolean("ok", false)) persist()
        return result
    }

    private fun persist() {
        runCatching { saveFile.writeText(NativeBridge.nativeSnapshot()) }
    }

    private fun obj(vararg pairs: Pair<String, Any?>): JSONObject {
        val o = JSONObject()
        for ((k, v) in pairs) o.put(k, v)
        return o
    }

    // ----- Nawyki ---------------------------------------------------------

    fun listHabits(): JSONArray =
        command(obj("op" to "habit.list").toString()).optJSONArray("habits") ?: JSONArray()

    fun habitStats(id: Long, today: Long): JSONObject =
        command(obj("op" to "habit.stats", "id" to id, "today" to today).toString())

    fun createHabit(name: String, description: String) {
        exec(
            obj(
                "op" to "habit.create",
                "name" to name,
                "description" to description,
                "icon" to "check",
                "daily" to true,
                "weekdayMask" to 0x7F,
            ).toString()
        )
    }

    fun setCompleted(id: Long, day: Long, done: Boolean) {
        exec(obj("op" to "habit.setCompleted", "id" to id, "day" to day, "done" to done).toString())
    }

    fun overview(today: Long): JSONObject =
        command(obj("op" to "overview", "today" to today).toString())

    // ----- Dziennik --------------------------------------------------------

    fun listEntries(): JSONArray =
        command("""{"op":"journal.list"}""").optJSONArray("entries") ?: JSONArray()

    fun createEntry(title: String, body: String, mood: Int, createdAtMs: Long) {
        exec(
            obj(
                "op" to "journal.create",
                "title" to title,
                "body" to body,
                "mood" to mood,
                "createdAtMs" to createdAtMs,
            ).toString()
        )
    }

    fun removeEntry(id: Long) {
        exec(obj("op" to "journal.remove", "id" to id).toString())
    }

    // ----- Nauka (SRS) -----------------------------------------------------

    fun dueCards(nowMs: Long, limit: Int): JSONArray =
        command(obj("op" to "card.due", "nowMs" to nowMs, "limit" to limit).toString())
            .optJSONArray("cards") ?: JSONArray()

    fun createCard(deck: String, front: String, back: String) {
        exec(obj("op" to "card.create", "deck" to deck, "front" to front, "back" to back).toString())
    }

    fun reviewCard(id: Long, quality: Int, nowMs: Long) {
        exec(obj("op" to "card.review", "id" to id, "quality" to quality, "nowMs" to nowMs).toString())
    }

    fun removeCard(id: Long) {
        exec(obj("op" to "card.remove", "id" to id).toString())
    }
}
