// Ekran „Dziennik": lista sesji medytacji + usuwanie.
// SPDX-License-Identifier: GPL-3.0-or-later
package pl.ascend.ui.screens

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.Card
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import pl.ascend.AppViewModel
import pl.ascend.R
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

@Composable
fun JournalScreen(viewModel: AppViewModel) {
    val sessions by viewModel.sessions.collectAsState()

    Column(Modifier.fillMaxSize().padding(16.dp)) {
        Text(stringResource(R.string.journal_title), style = MaterialTheme.typography.headlineMedium)
        Spacer(Modifier.height(12.dp))
        if (sessions.isEmpty()) {
            Text(stringResource(R.string.journal_empty), color = MaterialTheme.colorScheme.onSurfaceVariant)
        } else {
            LazyColumn(verticalArrangement = Arrangement.spacedBy(8.dp)) {
                items(sessions, key = { it.id }) { s ->
                    Card(Modifier.fillMaxWidth()) {
                        Column(Modifier.padding(12.dp)) {
                            Row(verticalAlignment = Alignment.CenterVertically) {
                                Text(
                                    stringResource(techniqueLabelRes(s.technique)),
                                    style = MaterialTheme.typography.titleMedium,
                                    modifier = Modifier.weight(1f),
                                )
                                Text(
                                    stringResource(R.string.journal_duration, (s.durationSeconds / 60).toInt()),
                                    style = MaterialTheme.typography.labelMedium,
                                    color = MaterialTheme.colorScheme.primary,
                                )
                            }
                            Spacer(Modifier.height(4.dp))
                            Text(
                                formatDate(s.startedAtMs),
                                style = MaterialTheme.typography.bodySmall,
                                color = MaterialTheme.colorScheme.onSurfaceVariant,
                            )
                            val moods = buildString {
                                s.moodBefore?.let { append(stringResource(moodLabelRes(it))) }
                                s.moodAfter?.let {
                                    if (isNotEmpty()) append(" → ")
                                    append(stringResource(moodLabelRes(it)))
                                }
                            }
                            if (moods.isNotBlank()) {
                                Spacer(Modifier.height(4.dp))
                                Text(moods, style = MaterialTheme.typography.bodyMedium)
                            }
                            if (s.notes.isNotBlank()) {
                                Spacer(Modifier.height(4.dp))
                                Text(s.notes, style = MaterialTheme.typography.bodyMedium)
                            }
                            Row(Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.End) {
                                TextButton(onClick = { viewModel.removeSession(s.id) }) {
                                    Text(stringResource(R.string.journal_delete))
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

internal fun moodLabelRes(mood: Int): Int = when (mood) {
    1 -> R.string.mood_awful
    2 -> R.string.mood_bad
    4 -> R.string.mood_good
    5 -> R.string.mood_great
    else -> R.string.mood_neutral
}

private fun formatDate(ms: Long): String {
    return SimpleDateFormat("d MMM yyyy, HH:mm", Locale.getDefault()).format(Date(ms))
}
