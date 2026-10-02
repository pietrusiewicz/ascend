// Ekran „Nauka": karty do powtórki (SRS) + przegląd/review.
// SPDX-License-Identifier: GPL-3.0-or-later
package pl.ascend.ui.screens

import androidx.compose.foundation.clickable
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
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.rememberScrollState
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Add
import androidx.compose.material3.AlertDialog
import androidx.compose.material3.Card
import androidx.compose.material3.FilledTonalButton
import androidx.compose.material3.FloatingActionButton
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.stringResource
import androidx.compose.ui.unit.dp
import pl.ascend.AppViewModel
import pl.ascend.CardItem
import pl.ascend.R

@Composable
fun LearnScreen(viewModel: AppViewModel) {
    val cards by viewModel.dueCards.collectAsState()
    var showAdd by remember { mutableStateOf(false) }
    var reviewing by remember { mutableStateOf<CardItem?>(null) }

    Box(Modifier.fillMaxSize()) {
        Column(Modifier.fillMaxSize().padding(16.dp)) {
            Text(stringResource(R.string.learn_title), style = MaterialTheme.typography.headlineMedium)
            Spacer(Modifier.height(12.dp))
            if (cards.isEmpty()) {
                Text(stringResource(R.string.learn_empty), color = MaterialTheme.colorScheme.onSurfaceVariant)
            } else {
                LazyColumn(verticalArrangement = Arrangement.spacedBy(8.dp)) {
                    items(cards, key = { it.id }) { c ->
                        Card(Modifier.fillMaxWidth().clickable { reviewing = c }) {
                            Column(Modifier.padding(12.dp)) {
                                if (c.deck.isNotBlank()) {
                                    Text(
                                        c.deck,
                                        style = MaterialTheme.typography.labelMedium,
                                        color = MaterialTheme.colorScheme.primary,
                                    )
                                }
                                Text(c.front, style = MaterialTheme.typography.bodyLarge)
                            }
                        }
                    }
                }
            }
        }

        FloatingActionButton(
            onClick = { showAdd = true },
            modifier = Modifier.align(Alignment.BottomEnd).padding(16.dp),
        ) {
            Icon(Icons.Filled.Add, contentDescription = stringResource(R.string.learn_add))
        }
    }

    if (showAdd) {
        AddCardDialog(
            onDismiss = { showAdd = false },
            onSave = { deck, front, back ->
                viewModel.addCard(deck, front, back)
                showAdd = false
            },
        )
    }

    reviewing?.let { card ->
        ReviewDialog(
            card = card,
            onDismiss = { reviewing = null },
            onRate = { quality ->
                viewModel.reviewCard(card.id, quality)
                reviewing = null
            },
        )
    }
}

@Composable
private fun AddCardDialog(onDismiss: () -> Unit, onSave: (String, String, String) -> Unit) {
    var deck by remember { mutableStateOf("") }
    var front by remember { mutableStateOf("") }
    var back by remember { mutableStateOf("") }

    AlertDialog(
        onDismissRequest = onDismiss,
        title = { Text(stringResource(R.string.learn_add)) },
        text = {
            Column {
                OutlinedTextField(
                    value = deck,
                    onValueChange = { deck = it },
                    label = { Text(stringResource(R.string.learn_deck)) },
                    singleLine = true,
                )
                Spacer(Modifier.height(8.dp))
                OutlinedTextField(
                    value = front,
                    onValueChange = { front = it },
                    label = { Text(stringResource(R.string.learn_front)) },
                )
                Spacer(Modifier.height(8.dp))
                OutlinedTextField(
                    value = back,
                    onValueChange = { back = it },
                    label = { Text(stringResource(R.string.learn_back)) },
                )
            }
        },
        confirmButton = {
            TextButton(
                onClick = { onSave(deck.trim(), front.trim(), back.trim()) },
                enabled = front.isNotBlank() && back.isNotBlank(),
            ) { Text(stringResource(R.string.save)) }
        },
        dismissButton = { TextButton(onClick = onDismiss) { Text(stringResource(R.string.cancel)) } },
    )
}

@Composable
private fun ReviewDialog(card: CardItem, onDismiss: () -> Unit, onRate: (Int) -> Unit) {
    var revealed by remember { mutableStateOf(false) }

    AlertDialog(
        onDismissRequest = onDismiss,
        title = { Text(card.front) },
        text = {
            Column {
                if (revealed) {
                    Text(card.back, style = MaterialTheme.typography.bodyLarge)
                    Spacer(Modifier.height(12.dp))
                    Row(Modifier.horizontalScroll(rememberScrollState())) {
                        RatingButton(stringResource(R.string.learn_again), 0, onRate)
                        RatingButton(stringResource(R.string.learn_hard), 3, onRate)
                        RatingButton(stringResource(R.string.learn_good), 4, onRate)
                        RatingButton(stringResource(R.string.learn_easy), 5, onRate)
                    }
                } else {
                    Text(stringResource(R.string.learn_show), color = MaterialTheme.colorScheme.primary)
                }
            }
        },
        confirmButton = {
            if (!revealed) {
                TextButton(onClick = { revealed = true }) { Text(stringResource(R.string.learn_show)) }
            }
        },
        dismissButton = { TextButton(onClick = onDismiss) { Text(stringResource(R.string.cancel)) } },
    )
}

@Composable
private fun RatingButton(label: String, quality: Int, onRate: (Int) -> Unit) {
    FilledTonalButton(onClick = { onRate(quality) }, modifier = Modifier.padding(end = 6.dp)) {
        Text(label)
    }
}
