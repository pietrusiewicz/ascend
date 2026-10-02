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

    // ----- Sesje -----------------------------------------------------------

    fun listSessions(): JSONArray =
        command("""{"op":"session.list"}""").optJSONArray("sessions") ?: JSONArray()

    fun createSession(
        startedAtMs: Long,
        durationSeconds: Long,
        technique: String,
        patternId: Long,
        moodBefore: Int?,
        moodAfter: Int?,
        notes: String,
    ) {
        val o = obj(
            "op" to "session.create",
            "startedAtMs" to startedAtMs,
            "durationSeconds" to durationSeconds,
            "technique" to technique,
            "patternId" to patternId,
            "notes" to notes,
        )
        if (moodBefore != null) o.put("moodBefore", moodBefore)
        if (moodAfter != null) o.put("moodAfter", moodAfter)
        exec(o.toString())
    }

    fun removeSession(id: Long) {
        exec(obj("op" to "session.remove", "id" to id).toString())
    }

    // ----- Wzorce oddechu ----------------------------------------------------

    fun listPatterns(): JSONArray =
        command("""{"op":"pattern.list"}""").optJSONArray("patterns") ?: JSONArray()

    // ----- Statystyki --------------------------------------------------------

    fun stats(today: Long): JSONObject =
        command(obj("op" to "stats", "today" to today).toString())
}
