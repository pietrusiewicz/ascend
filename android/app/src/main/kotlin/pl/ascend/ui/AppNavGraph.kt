// Dolna nawigacja: Medytuj / Dziennik / Statystyki.
// SPDX-License-Identifier: GPL-3.0-or-later
package pl.ascend.ui

import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.padding
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.BarChart
import androidx.compose.material.icons.filled.Book
import androidx.compose.material.icons.filled.SelfImprovement
import androidx.compose.material3.Icon
import androidx.compose.material3.NavigationBar
import androidx.compose.material3.NavigationBarItem
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.saveable.rememberSaveable
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.res.stringResource
import pl.ascend.AppViewModel
import pl.ascend.R
import pl.ascend.ui.screens.JournalScreen
import pl.ascend.ui.screens.MeditateScreen
import pl.ascend.ui.screens.StatsScreen

private data class Tab(val labelRes: Int, val icon: ImageVector)

@Composable
fun AppNavGraph(viewModel: AppViewModel) {
    val tabs = listOf(
        Tab(R.string.tab_meditate, Icons.Filled.SelfImprovement),
        Tab(R.string.tab_journal, Icons.Filled.Book),
        Tab(R.string.tab_stats, Icons.Filled.BarChart),
    )
    var selected by rememberSaveable { mutableStateOf(0) }

    Scaffold(
        bottomBar = {
            NavigationBar {
                tabs.forEachIndexed { index, tab ->
                    NavigationBarItem(
                        selected = selected == index,
                        onClick = { selected = index },
                        icon = { Icon(tab.icon, contentDescription = stringResource(tab.labelRes)) },
                        label = { Text(stringResource(tab.labelRes)) },
                    )
                }
            }
        },
    ) { padding ->
        Box(Modifier.padding(padding)) {
            when (selected) {
                0 -> MeditateScreen(viewModel)
                1 -> JournalScreen(viewModel)
                else -> StatsScreen(viewModel)
            }
        }
    }
}
