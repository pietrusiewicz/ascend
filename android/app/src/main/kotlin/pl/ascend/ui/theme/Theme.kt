// Motyw Material 3 aplikacji.
// SPDX-License-Identifier: GPL-3.0-or-later
package pl.ascend.ui.theme

import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.lightColorScheme
import androidx.compose.runtime.Composable
import androidx.compose.ui.graphics.Color

private val LightColors = lightColorScheme(
    primary = Color(0xFF4F46E5),
    onPrimary = Color.White,
    secondary = Color(0xFF6D28D9),
    tertiary = Color(0xFF0EA5E9),
)

@Composable
fun AscendTheme(content: @Composable () -> Unit) {
    MaterialTheme(colorScheme = LightColors, content = content)
}
