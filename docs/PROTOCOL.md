# Protokół kanału poleceń (JNI `command`)

Fasada rdzenia przyjmuje obiekt JSON z polem `op` i zwraca obiekt JSON
z polem `ok` (true/false). Błędy zwracają `{"ok": false, "error": "..."}`.

Identyfikatory są `uint64`, czas w ms epoki Unix (UTC), dzień = `ms / 86_400_000`.

## Metainformacje

| op | wejście | wynik |
|----|---------|-------|
| `ping` | — | `{pong:true}` |
| `version` | — | `{version, protocol, schema}` |
| `overview` | `today` (dzień) | `{activeHabits, doneToday, dueCards, entries}` |

## Nawyki

| op | wejście | wynik |
|----|---------|-------|
| `habit.list` | `includeArchived?` | `{habits:[...]}` |
| `habit.get` | `id` | `{habit:{...}}` |
| `habit.create` | `name`, `description?`, `icon?`, `color?`, `daily?`, `weekdayMask?`, `createdAtMs?` | `{habit:{...}}` |
| `habit.update` | `id` + dowolne pola | `{habit:{...}}` |
| `habit.archive` | `id`, `archived` | `{ok}` |
| `habit.setCompleted` | `id`, `day`, `done` | `{ok}` |
| `habit.stats` | `id`, `today` | `{currentStreak, longestStreak, doneToday}` |

Kształt `habit`: `{id, name, description, icon, color, daily, weekdayMask, createdAtMs, archived}`.
`weekdayMask`: bity 0–6 = poniedziałek–niedziela (gdy `daily=false`).

## Dziennik

| op | wejście | wynik |
|----|---------|-------|
| `journal.list` | — | `{entries:[...]}` (malejąco wg czasu) |
| `journal.get` | `id` | `{entry:{...}}` |
| `journal.create` | `title`, `body?`, `mood?` (1–5), `createdAtMs?` | `{entry:{...}}` |
| `journal.update` | `id` + dowolne pola | `{entry:{...}}` |
| `journal.remove` | `id` | `{ok}` |

Kształt `entry`: `{id, createdAtMs, editedAtMs, title, body, mood}`.

## Nauka (SM-2)

| op | wejście | wynik |
|----|---------|-------|
| `card.list` | — | `{cards:[...]}` |
| `card.get` | `id` | `{card:{...}}` |
| `card.create` | `deck?`, `front`, `back` | `{card:{...}}` |
| `card.update` | `id` + `deck?`/`front?`/`back?` | `{card:{...}}` |
| `card.remove` | `id` | `{ok}` |
| `card.review` | `id`, `quality` (0–5), `nowMs` | `{card:{...}}` (zaktualizowana) |
| `card.due` | `nowMs`, `limit?` | `{cards:[...]}` (dueMs ≤ nowMs, rosnąco) |

Kształt `card`: `{id, deck, front, back, dueMs, intervalDays, ease, repetitions, lapses}`.

## Snapshot (format zapisu v1)

```json
{
  "schema": 1, "protocol": 1,
  "nextHabitId": 1, "nextEntryId": 1, "nextCardId": 1,
  "habits": [], "completions": [], "journal": [], "cards": []
}
```
