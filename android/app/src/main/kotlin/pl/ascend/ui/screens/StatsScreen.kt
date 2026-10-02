// Ekran „Statystyki": passa, liczba sesji, łączny czas.
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
import androidx.compose.material3.Card
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import pl.ascend.AppViewModel
import pl.ascend.R

@Composable
fun StatsScreen(viewModel: AppViewModel) {
    val stats by viewModel.stats.collectAsState()

    Column(Modifier.fillMaxSize().padding(16.dp)) {
        Text(stringResource(R.string.stats_title), style = MaterialTheme.typography.headlineMedium)
        Spacer(Modifier.height(16.dp))

        Row(Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(12.dp)) {
            StatCard(
                stringResource(R.string.stats_streak),
                stringResource(R.string.stats_streak_days, stats.streakDays),
                Modifier.weight(1f),
            )
            StatCard(
                stringResource(R.string.stats_longest),
                stringResource(R.string.stats_streak_days, stats.longestStreak),
                Modifier.weight(1f),
            )
        }
        Spacer(Modifier.height(12.dp))
        Row(Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.spacedBy(12.dp)) {
            StatCard(
                stringResource(R.string.stats_total_sessions),
                stats.totalSessions.toString(),
                Modifier.weight(1f),
            )
            StatCard(
                stringResource(R.string.stats_total_time),
                stringResource(R.string.stats_minutes, (stats.totalSeconds / 60).toInt()),
                Modifier.weight(1f),
            )
        }
        Spacer(Modifier.height(12.dp))
        StatCard(
            stringResource(R.string.stats_today),
            stringResource(R.string.stats_today_sessions, stats.sessionsToday) +
                " · " + stringResource(R.string.stats_minutes, (stats.secondsToday / 60).toInt()),
            Modifier.fillMaxWidth(),
        )
    }
}

@Composable
private fun StatCard(label: String, value: String, modifier: Modifier = Modifier) {
    Card(modifier) {
        Column(Modifier.padding(16.dp)) {
            Text(value, style = MaterialTheme.typography.headlineSmall)
            Text(
                label,
                style = MaterialTheme.typography.bodySmall,
                color = MaterialTheme.colorScheme.onSurfaceVariant,
            )
        }
    }
}
