// Ekran „Dziś": podsumowanie + lista dzisiejszych nawyków.
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
import androidx.compose.foundation.layout.width
import androidx.compose.material3.Card
import androidx.compose.material3.Checkbox
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import pl.ascend.AppViewModel
import pl.ascend.R

@Composable
fun HomeScreen(viewModel: AppViewModel) {
    val overview by viewModel.overview.collectAsState()
    val habits by viewModel.habits.collectAsState()

    Column(Modifier.fillMaxSize().padding(16.dp)) {
        Text(stringResource(R.string.home_title), style = MaterialTheme.typography.headlineMedium)
        Spacer(Modifier.height(16.dp))

        Row(Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(12.dp)) {
            StatCard(stringResource(R.string.overview_habits), overview.activeHabits, Modifier.weight(1f))
            StatCard(stringResource(R.string.overview_done), overview.doneToday, Modifier.weight(1f))
        }
        Spacer(Modifier.height(12.dp))
        Row(Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(12.dp)) {
            StatCard(stringResource(R.string.overview_due), overview.dueCards, Modifier.weight(1f))
            StatCard(stringResource(R.string.overview_entries), overview.entries, Modifier.weight(1f))
        }

        Spacer(Modifier.height(24.dp))
        Text(stringResource(R.string.tab_habits), style = MaterialTheme.typography.titleLarge)
        Spacer(Modifier.height(8.dp))

        if (habits.isEmpty()) {
            Text(stringResource(R.string.habits_empty), color = MaterialTheme.colorScheme.onSurfaceVariant)
        } else {
            habits.forEach { h ->
                Row(
                    Modifier.fillMaxWidth().padding(vertical = 4.dp),
                    verticalAlignment = Alignment.CenterVertically,
                ) {
                    Checkbox(checked = h.doneToday, onCheckedChange = { viewModel.toggleHabit(h.id) })
                    Spacer(Modifier.width(8.dp))
                    Column {
                        Text(h.name, style = MaterialTheme.typography.bodyLarge)
                        Text(
                            stringResource(R.string.streak_current, h.currentStreak),
                            style = MaterialTheme.typography.bodySmall,
                            color = MaterialTheme.colorScheme.onSurfaceVariant,
                        )
                    }
                }
            }
        }
    }
}

@Composable
private fun StatCard(label: String, value: Int, modifier: Modifier = Modifier) {
    Card(modifier) {
        Column(Modifier.padding(16.dp)) {
            Text(value.toString(), style = MaterialTheme.typography.headlineSmall)
            Text(
                label,
                style = MaterialTheme.typography.bodySmall,
                color = MaterialTheme.colorScheme.onSurfaceVariant,
            )
        }
    }
}
