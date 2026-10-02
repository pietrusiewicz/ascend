// Ekran „Medytuj": timer + prowadzenie oddechu (wzorzec) + zapis sesji.
// SPDX-License-Identifier: GPL-3.0-or-later
package pl.ascend.ui.screens

import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.Button
import androidx.compose.material3.FilterChip
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedButton
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import kotlinx.coroutines.delay
import pl.ascend.AppViewModel
import pl.ascend.PatternItem
import pl.ascend.R

private enum class BreathPhase { Inhale, HoldIn, Exhale, HoldOut }

private val durations = listOf(0, 1, 5, 10, 15, 20)

private val techniques = listOf(
    "breath", "silence", "body_scan", "walking", "mantra", "guided",
)

internal fun techniqueLabelRes(technique: String): Int = when (technique) {
    "silence" -> R.string.technique_silence
    "body_scan" -> R.string.technique_body_scan
    "walking" -> R.string.technique_walking
    "mantra" -> R.string.technique_mantra
    "guided" -> R.string.technique_guided
    else -> R.string.technique_breath
}

private fun phaseAt(pattern: PatternItem, elapsedMs: Long): Pair<BreathPhase, Float> {
    val cycle = (pattern.cycleSeconds * 1000L).coerceAtLeast(1)
    val t = elapsedMs % cycle
    val inhale = pattern.inhaleSeconds * 1000L
    val holdIn = pattern.holdInSeconds * 1000L
    val exhale = pattern.exhaleSeconds * 1000L

    return when {
        t < inhale -> BreathPhase.Inhale to (t.toFloat() / inhale.coerceAtLeast(1))
        t < inhale + holdIn -> BreathPhase.HoldIn to 1f
        t < inhale + holdIn + exhale -> {
            val p = (t - inhale - holdIn).toFloat() / exhale.coerceAtLeast(1)
            BreathPhase.Exhale to p
        }
        else -> BreathPhase.HoldOut to 1f
    }
}

private fun phaseLabel(phase: BreathPhase): Int = when (phase) {
    BreathPhase.Inhale -> R.string.meditate_breathe_in
    BreathPhase.HoldIn -> R.string.meditate_hold
    BreathPhase.Exhale -> R.string.meditate_breathe_out
    BreathPhase.HoldOut -> R.string.meditate_hold
}

@Composable
fun MeditateScreen(viewModel: AppViewModel) {
    val patterns by viewModel.patterns.collectAsState()

    var selectedPatternId by rememberSaveable { mutableStateOf(1L) }
    var technique by rememberSaveable { mutableStateOf("breath") }
    var durationMin by rememberSaveable { mutableStateOf(0) }

    var running by remember { mutableStateOf(false) }
    var startedAtMs by remember { mutableStateOf(0L) }
    var elapsedMs by remember { mutableStateOf(0L) }
    var showSave by remember { mutableStateOf(false) }

    val pattern = patterns.firstOrNull { it.id == selectedPatternId } ?: patterns.firstOrNull()

    LaunchedEffect(running) {
        while (running) {
            elapsedMs = System.currentTimeMillis() - startedAtMs
            if (durationMin > 0 && elapsedMs >= durationMin * 60_000L) {
                running = false
                showSave = true
                break
            }
            delay(80)
        }
    }

    Column(
        Modifier.fillMaxSize().padding(16.dp),
        horizontalAlignment = Alignment.CenterHorizontally,
    ) {
        Text(stringResource(R.string.meditate_title), style = MaterialTheme.typography.headlineMedium)
        Spacer(Modifier.height(16.dp))

        // Wzorzec oddechu
        Text(stringResource(R.string.meditate_pattern), style = MaterialTheme.typography.titleMedium)
        Spacer(Modifier.height(4.dp))
        Row(Modifier.horizontalScroll(rememberScrollState())) {
            patterns.forEach { p ->
                FilterChip(
                    selected = p.id == selectedPatternId,
                    onClick = { selectedPatternId = p.id },
                    label = { Text(p.name) },
                    modifier = Modifier.padding(end = 6.dp),
                )
            }
        }
        Spacer(Modifier.height(8.dp))

        // Technika
        Text(stringResource(R.string.meditate_technique), style = MaterialTheme.typography.titleMedium)
        Spacer(Modifier.height(4.dp))
        Row(Modifier.horizontalScroll(rememberScrollState())) {
            techniques.forEach { t ->
                FilterChip(
                    selected = t == technique,
                    onClick = { technique = t },
                    label = { Text(stringResource(techniqueLabelRes(t))) },
                    modifier = Modifier.padding(end = 6.dp),
                )
            }
        }
        Spacer(Modifier.height(8.dp))

        // Czas trwania
        Text(stringResource(R.string.meditate_duration), style = MaterialTheme.typography.titleMedium)
        Spacer(Modifier.height(4.dp))
        Row(Modifier.horizontalScroll(rememberScrollState())) {
            durations.forEach { d ->
                FilterChip(
                    selected = d == durationMin,
                    onClick = { durationMin = d },
                    label = {
                        Text(
                            if (d == 0) stringResource(R.string.meditate_duration_free)
                            else "$d min"
                        )
                    },
                    modifier = Modifier.padding(end = 6.dp),
                )
            }
        }

        Spacer(Modifier.height(24.dp))

        // Kółko oddechu
        if (pattern != null) {
            val (phase, progress) = if (running) phaseAt(pattern, elapsedMs) else BreathPhase.Inhale to 0f
            val scale = when (phase) {
                BreathPhase.Inhale -> 0.45f + 0.55f * progress
                BreathPhase.Exhale -> 1f - 0.55f * progress
                BreathPhase.HoldIn, BreathPhase.HoldOut -> 1f
            }
            val base = 220.dp
            val size = base * (0.5f + 0.5f * scale)
            Box(
                Modifier
                    .size(size)
                    .clip(CircleShape)
                    .background(MaterialTheme.colorScheme.primary),
                contentAlignment = Alignment.Center,
            ) {
                Text(
                    stringResource(phaseLabel(phase)),
                    color = MaterialTheme.colorScheme.onPrimary,
                    style = MaterialTheme.typography.titleMedium,
                )
            }
            Spacer(Modifier.height(16.dp))
        }

        // Zegar
        Text(formatTime(elapsedMs), style = MaterialTheme.typography.displaySmall)
        Spacer(Modifier.height(24.dp))

        Row(horizontalArrangement = Arrangement.spacedBy(12.dp)) {
            if (!running) {
                Button(onClick = {
                    startedAtMs = System.currentTimeMillis()
                    elapsedMs = 0
                    running = true
                }) { Text(stringResource(R.string.meditate_start)) }
            } else {
                OutlinedButton(onClick = {
                    running = false
                    showSave = true
                }) { Text(stringResource(R.string.meditate_stop)) }
            }
        }
    }

    if (showSave) {
        val seconds = (elapsedMs / 1000L).coerceAtLeast(1)
        SaveSessionDialog(
            onDismiss = { showSave = false },
            onSave = { before, after, notes ->
                viewModel.saveSession(
                    startedAtMs = startedAtMs,
                    durationSeconds = seconds,
                    technique = technique,
                    patternId = pattern?.id ?: 0L,
                    moodBefore = before,
                    moodAfter = after,
                    notes = notes,
                )
                showSave = false
            },
        )
    }
}

@Composable
private fun SaveSessionDialog(
    onDismiss: () -> Unit,
    onSave: (Int?, Int?, String) -> Unit,
) {
    var before by remember { mutableStateOf(0) }
    var after by remember { mutableStateOf(0) }
    var notes by remember { mutableStateOf("") }

    AlertDialog(
        onDismissRequest = onDismiss,
        title = { Text(stringResource(R.string.meditate_save_title)) },
        text = {
            Column {
                Text(stringResource(R.string.meditate_mood_before), style = MaterialTheme.typography.labelLarge)
                Row(Modifier.horizontalScroll(rememberScrollState())) {
                    for (m in 1..5) {
                        FilterChip(
                            selected = before == m,
                            onClick = { before = if (before == m) 0 else m },
                            label = { Text(stringResource(moodLabelRes(m))) },
                            modifier = Modifier.padding(end = 4.dp),
                        )
                    }
                }
                Spacer(Modifier.height(8.dp))
                Text(stringResource(R.string.meditate_mood_after), style = MaterialTheme.typography.labelLarge)
                Row(Modifier.horizontalScroll(rememberScrollState())) {
                    for (m in 1..5) {
                        FilterChip(
                            selected = after == m,
                            onClick = { after = if (after == m) 0 else m },
                            label = { Text(stringResource(moodLabelRes(m))) },
                            modifier = Modifier.padding(end = 4.dp),
                        )
                    }
                }
                Spacer(Modifier.height(8.dp))
                OutlinedTextField(
                    value = notes,
                    onValueChange = { notes = it },
                    label = { Text(stringResource(R.string.meditate_notes)) },
                    placeholder = { Text(stringResource(R.string.meditate_notes_hint)) },
                    modifier = Modifier.fillMaxWidth(),
                )
            }
        },
        confirmButton = {
            TextButton(onClick = {
                onSave(
                    before.takeIf { it in 1..5 },
                    after.takeIf { it in 1..5 },
                    notes.trim(),
                )
            }) { Text(stringResource(R.string.save)) }
        },
        dismissButton = { TextButton(onClick = onDismiss) { Text(stringResource(R.string.cancel)) } },
    )
}

private fun formatTime(ms: Long): String {
    val totalSec = ms / 1000L
    val m = totalSec / 60
    val s = totalSec % 60
    return "%02d:%02d".format(m, s)
}
