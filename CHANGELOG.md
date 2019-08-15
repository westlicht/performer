# Changelog

## v0.1.31 (15 Aug 2019)

#### Improvements

- Added _Slide Time_ parameter to MIDI/CV track (#121)
- Added _Transpose_ parameter to MIDI/CV track (#128)
- Allow MIDI/CV tracks to be muted

#### Fixes

- Use _Last Note_ note priority as default value on MIDI/CV track
- Added swing to arpeggiator on MIDI/CV track

## v0.1.30 (11 Aug 2019)

This release is mostly dedicated to improving song mode. Many things have changed, please consult the manual for learning how to use it.

#### Improvements

- Added _Time Signature_ on project page for setting the duration of a bar (#118)
- Increased the number of song slots to 64 (#118)
- Added context menu to song page containing the _Init_ function (#118)
- Added ability to duplicate song slots (#118)
- Show song slots in a table (#118)
- Allow song slots to play for up to 128 bars (#118)
- Improved octave playback for arpeggiator (#109)

#### Fixes

- Do not reset MIDI/CV tracks when switching slots during song playback
- Fixed some edge cases during song playback

## v0.1.29 (31 Jul 2019)

#### Improvements

- Added ability to select all steps with an equal value on the selected layer using `SHIFT` + double tap a step (#117)
- Added _Note Priority_ parameter to MIDI/CV track (#115)

## v0.1.28 (28 Jul 2019)

#### Improvements

- Allow editing first/last step parameter together (#114)

#### Fixes

- Fixed rare edge case where first/last step parameter could end up in invalid state and crash the firmware
- Fixed glitchy encoder by using a full state machine decoding the encoders gray code (#112)

## v0.1.27 (24 Jul 2019)

This release primarily focuses on improvements of curve tracks.

#### Improvements

- Added _shape variation_ and _shape variation probability_ layers to curve sequences (#108)
- Added _gate_ and _gate probability_ layer to curve sequences (#108)
- Added _shape probability bias_ and _gate probability bias_ parameters to curve tracks (#108)
- Added better support for track linking on curve tracks including recording (#108)
- Added specific fill modes for curve tracks (variation, next pattern and invert) (#108)
- Added support for mutes on curve tracks (keep CV value at last position and mute gates) (#108)
- Added support for offsetting curves up and down by holding min or max function buttons (#103)

## v0.1.26 (20 Jul 2019)

#### Improvements

- Evaluate transposition (octave/transpose) when monitoring and recording notes (#102)
- Added routing target for tap tempo (#100)

## v0.1.25 (18 Jul 2019)

#### Improvements

- Allow chromatic note values to be changed in octaves by holding `SHIFT` (#101)
- Swapped high and low curve types and default curve sequence to be all low curves (#101)
- Show active step on all layers during step recording

#### Fixes

- Fixed euclidean generator from being destructive (#97)
- Fixed handling of active routing targets (#98)
- Fixed some edge cases in the step selection UI
- Fixed ghost live recording from past events
- Fixed condition layer value editing (clamp to valid values)

## v0.1.24 (9 Jul 2019)

#### Fixes

- Fixed writing incorrect project version

## v0.1.23 (8 Jul 2019)

**PLEASE DO NOT USE THIS VERSION, IT WRITES THE INCORRECT PROJECT FILE VERSION!**

#### Improvements

- Check and disallow to create conflicting routes (#95)
- Added new _Note Range_ MIDI event type for routes (#96)

#### Fixes

- Fixed track selection for new routing targets added in previous release
- Fixed hidden editing of route tracks

## v0.1.22 (6 Jul 2019)

#### Improvements

- Pressing `SHIFT` + `F[1-5]` on the sequence edit pages selects the first layer type of the group (#84)
- Solo a track is now done with `SHIFT` + `T[1-8]` on the performer page (#87)
- Added ability to hold fills by pressing `SHIFT` + `S[9-16]` on the performer page (#65)
- Added new _Fill Amount_ parameter to control the amount of fill in effect (#65)
- Added new routing targets: Mute, Fill, Fill Amount and Pattern (#89)
- Added new _Condition_ layer to note sequences to conditionally trigger steps (#13)
- Do not show divisor denominator if it is 1

#### Fixes

- Fixed unreliable live recording of note sequences (#63)
- Fixed latching mode when selecting a new global pattern on pattern page

## v0.1.21 (26 Jun 2019)

#### Improvements

- Added recording curve tracks from a CV source (#72)
- Added ability to output analog clock with swing (#5)
- Added sequence progress info to performer page (#79)

#### Fixes

- Fixed navigation grid on launchpad when note/slide layer is selected

## v0.1.20 (15 Jun 2019)

This is mostly a bug fix release. From this release onwards, binaries are only available in intel hex format, which is probably what most people used anyway.

#### Improvements

- Allow larger range of divisor values to enable even slower sequences (#61)
- Added hwconfig option for inverting LEDs (#74)

#### Fixes

- Fixed changing multiple track modes at once on layout page
- Fixed MIDI channel info on monitor page (show channel as 1-16 instead of 0-15)
- Fixed "hidden commit button" on layout page (#68)
- Fixed race condition when initializing a project
- Fixed detecting button up events on launchpad devices

## v0.1.19 (18 May 2019)

#### Improvements

- Added arpeggiator to MIDI/CV track (#47)
- Show note names for chromatic scales (#55)
- Added divisor as a routable target (#53)
- Improved free play mode to react smoothly on divisor changes (#53)

#### Fixes

- Handle _alternative_ note off MIDI events (e.g. note on events with velocity 0) (#56)
- Update linked tracks immediately not only when track mode is changed

## v0.1.18 (8 May 2019)

#### Improvements

- Added _Slide Time_ parameter to curve tracks to allow for slew limiting (#40)

#### Fixes

- Fixed delayed CV output signals, now CV signal is guaranteed to be updated **before** gate signal (#36)

## v0.1.17 (7 May 2019)

**Note: The new features added by this release are not yet heavily tested. You can always downgrade should you experience problems, but please report them.**

#### Improvements

- Added gate offset to note sequences, allowing to delay gates (#14)
- Added watchdog supervision (#31)

## v0.1.16 (5 May 2019)

#### Improvements

- Improved UI for editing MIDI port/channel (#39)

#### Fixes

- Fixed tied notes when output via MIDI (#38)
- Fixed regression updating gate outputs in tester application (#37)

## v0.1.15 (20 Apr 2019)

#### Improvements

- Added CV/Gate to MIDI conversion to allow using a CV/Gate source for recording (#32)

#### Fixes

- Fixed setting gate outputs before CV outputs (#36)

## v0.1.14 (17 Apr 2019)

#### Improvements

- Removed switching launchpad brightness in favor of full brightness
- Enhanced launchpad pattern page
    - Show patterns that are not empty with dimmed colors (note patterns in dim yellow, curve patterns in dim red) (#25, #26)
    - Show requested patterns (latched or synched) with dim green

#### Fixes

- Fixed saving/loading user scales as part of the project
- Fixed setting name when loading user scales (#34)

## v0.1.13 (15 Apr 2019)

#### Fixes

- Fixed Launchpad Pro support
- Fixed _Reset Measure_ when _Play Mode_ is set to _Aligned_ (#33)

## v0.1.12 (7 Apr 2019)

#### Improvements

- Refactored routing system to be non-destructive (this means that parameters will jump back to the original value when a route is deleted) (#27)
- _First Step_, _Last Step_ and _Run Mode_ routes affect every pattern on a track (instead of just the first one)
- Indicate routed parameters in the UI with an right facing arrow
- Added `ROUTE` item in context menu of sequence pages to allow creating routes (as in other pages)
- Added support for Launchpad Pro

## v0.1.11 (3 Apr 2019)

This release is mostly adding support for additional launchpad devices. However, while testing I unfortunately found that the hardware might have an issue on the USB power supply (in fact there is quite some ripple on the internal 5V power rail) leading to spurious resets of the launchpad. Connecting the launchpad through an externally powered USB hub seems to fix the issue. Therefore it is beneficial to run the launchpad with dimmed leds (default). For now, the brightness can be adjusted by pressing `8` + `6` on the launchpad. I will investigate the hardware issue as soon as possible.

#### Improvements

- Added preliminary support for Launchpad S and Launchpad Mk2

#### Fixes

- Fixed launchpad display regression (some sequence data was displayed in wrong colors)
- Fixed fill regression (fills did not work if track was muted anymore)

## v0.1.10 (1 Apr 2019)

#### Improvements

- Implemented _Step Record_ mode (#23)

## v0.1.9 (29 Mar 2019)

#### Improvements

- Improved editing curve min/max value on Launchpad (#19)
- Show octave base notes on Launchpad when editing notes (#18)
- Allow selecting sequence run mode on Launchpad

#### Fixes

- Fixed stuck notes on MIDI output

## v0.1.8 (27 Mar 2019)

#### Improvements

- Added `CV Update Mode` to note track settings to select between updating CV only on gates or always
- Added ability to use curve tracks as sources for MIDI output

#### Fixes

- Fixed copying curve tracks to clipboard (resulted in crash)

## v0.1.7 (23 Mar 2019)

#### Fixes

- Fixed glitchy encoder (#4)
- Implemented legato notes and slides via MIDI output (#3)
- Fixed routing and MIDI output page when clearing or loading a project

## v0.1.6 (17 Mar 2019)

#### Fixes

- Fixed MIDI output (note and velocity constant values were off by one before)

## v0.1.5 (1 Feb 2019)

### !!! This version is not backward compatible with previous project files !!!

#### Improvements

- `Run Mode` of sequences is now a routing target
- Added retrigger and note probability bias to track parameters and routing targets
- All probability values are now in the range of 12.5% to 100%
- Nicer visualization of target tracks in routing page
- Allow clock output pulse length of up to 20ms

## v0.1.4 (24 Jan 2019)

#### Improvements

- Rearranged quick edit of sequence parameters due to changes in frontpanel
- Changed the preset scales
- Note values are now displayed as note index and octave instead of note value and octave
- Flipped encoder reverse mode in hardware config to make default encoder in BOM act correctly
- Flipped encoder direction in bootloader

#### Bugfixes

- Fixed clock reset output state when device is powered on
- Fixed tap tempo
- Fixed pattern page when mutes are activated
- Fixed clock input/output divisors

## v0.1.3 (14 Nov 2018)

#### New Features

- Added MIDI output module
    - Generate Note On/Off events from gate, pitch and velocity of different tracks (or constant values)
    - Generate CC events from track CV
- Overview page
    - Press `PAGE` + `LEFT` to open Overview page
- Simple startup screen that automatically loads the last used project on power on
- Added new routing targets for controlling Play and Record

#### Improvements

- Sequence Edit page is now called Steps page
- Show track mode on Sequence page (same as on Track page)
- Improved route ranges
    - Allow +/- 60 semitone transposition
- Restyled pattern page
    - Differentiate from Performer page
    - Show selected pattern per track
- Exit editing mode when committing a Route or MIDI Output

#### Bugfixes

- Fixed exponential curves (do not jump to high value at the end)
- Fixed loading project name
- Fixed showing pages on leds
- Fixed launching patterns in latched mode
- Fixed setting edit pattern when changing patterns on Launchpad

## v0.1.2 (5 Nov 2018)

- No notes

## v0.1.1 (2 Oct 2018)

- Initial release
