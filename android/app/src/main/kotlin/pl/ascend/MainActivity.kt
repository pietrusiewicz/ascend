// Jedyna aktywność aplikacji — hostuje UI Compose.
// SPDX-License-Identifier: GPL-3.0-or-later
package pl.ascend

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.viewModels
import pl.ascend.ui.AppNavGraph
import pl.ascend.ui.theme.AscendTheme

class MainActivity : ComponentActivity() {

    private val viewModel: AppViewModel by viewModels()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            AscendTheme {
                AppNavGraph(viewModel)
            }
        }
    }
}
