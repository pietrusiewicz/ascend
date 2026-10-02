# Protokół kanału poleceń (JNI `command`)

Fasada rdzenia przyjmuje obiekt JSON z polem `op` i zwraca obiekt JSON
z polem `ok` (true/false). Błędy zwracają `{"ok": false, "error": "..."}`.

Identyfikatory są `uint64`, czas w ms epoki Unix (UTC), dzień = `ms / 86_400_000`.
Technika to string: `breath`, `silence`, `body_scan`, `walking`, `mantra`, `guided`.

## Metainformacje

| op | wejście | wynik |
|----|---------|-------|
| `ping` | — | `{pong:true}` |
| `version` | — | `{version, protocol, schema}` |
| `stats` | `today` (dzień) | `{streakDays, longestStreak, totalSessions, totalSeconds, sessionsToday, secondsToday}` |
| `overview` | `today` | alias dla `stats` |

## Sesje

| op | wejście | wynik |
|----|---------|-------|
| `session.list` | — | `{sessions:[...]}` (malejąco wg `startedAtMs`) |
| `session.get` | `id` | `{session:{...}}` |
| `session.create` | `startedAtMs`, `durationSeconds` (>0), `technique?`, `patternId?`, `moodBefore?` (1–5), `moodAfter?` (1–5), `notes?` | `{session:{...}}` |
| `session.update` | `id` + dowolne pola | `{session:{...}}` |
| `session.remove` | `id` | `{ok}` |

Kształt `session`: `{id, startedAtMs, durationSeconds, technique, patternId, notes, moodBefore?, moodAfter?}`
(`moodBefore`/`moodAfter` obecne tylko, gdy ustawione).

## Wzorce oddechu

| op | wejście | wynik |
|----|---------|-------|
| `pattern.list` | — | `{patterns:[...]}` (wbudowane najpierw, potem własne) |
| `pattern.create` | `name`, `inhaleSeconds` (>0), `exhaleSeconds` (>0), `holdInSeconds?`, `holdOutSeconds?` | `{pattern:{...}}` |
| `pattern.update` | `id` + dowolne pola | `{pattern:{...}}` |
| `pattern.remove` | `id` | `{ok}` (wbudowanych nie usuwamy) |

Kształt `pattern`: `{id, name, inhaleSeconds, holdInSeconds, exhaleSeconds, holdOutSeconds, builtin}`.
Wbudowane mają `id` 1–4 i `builtin:true`; własne `id ≥ 100`.

## Snapshot (format zapisu v2)

```json
{
  "schema": 2, "protocol": 2,
  "nextSessionId": 1, "nextPatternId": 100,
  "sessions": [], "patterns": []
}
```
