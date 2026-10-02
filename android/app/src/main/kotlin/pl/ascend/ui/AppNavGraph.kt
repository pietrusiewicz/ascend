// Dolna nawigacja: Dziś / Nawyki / Dziennik / Nauka.
// SPDX-License-Identifier: GPL-3.0-or-later
package pl.ascend.ui

import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.padding
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Check
import androidx.compose.material.icons.filled.Edit
import androidx.compose.material.icons.filled.Home
import androidx.compose.material.icons.filled.Star
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
import pl.ascend.ui.screens.HabitsScreen
import pl.ascend.ui.screens.HomeScreen
import pl.ascend.ui.screens.JournalScreen
import pl.ascend.ui.screens.LearnScreen

private data class Tab(val labelRes: Int, val icon: ImageVector)

@Composable
fun AppNavGraph(viewModel: AppViewModel) {
    val tabs = listOf(
        Tab(R.string.tab_home, Icons.Filled.Home),
        Tab(R.string.tab_habits, Icons.Filled.Check),
        Tab(R.string.tab_journal, Icons.Filled.Edit),
        Tab(R.string.tab_learn, Icons.Filled.Star),
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
                0 -> HomeScreen(viewModel)
                1 -> HabitsScreen(viewModel)
                2 -> JournalScreen(viewModel)
                else -> LearnScreen(viewModel)
            }
        }
    }
}
