# Improvement: MIDI Improvements

This change improves MIDI integration of the performer.

## Program Change Messages

The performer can now send and receive program change messages.

Note:

- Program change messages must be enabled in the project settings
- Program change messages are only sent when changing all patterns to the same pattern (changing patterns individually will not trigger program change messages)
- Program change messages are currently always sent on channel 0
- Program change messages are sent slightly before the end of the sequence in sync mode (this allows the receiving hardware some time to acknowledge the program change request)

## Always Sync Patterns

Enabled in project settings. This isn't strictly a MIDI improvement, but it works well with program changes.

When enabled, pattern changes are synced by default, rather than having to press **SYNC** before selecting a pattern.

Not very exciting, but very useful for live performance when you want things to always be synced.