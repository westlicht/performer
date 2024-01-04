# Changelog

## v 0.1.46 (4 January 2024)

- UI note edit page reaggment
- song sync with clock
- fix reverse shape feature
- add new shapes
- add new repeat modes
- encoder to change tempo on Performer page

## v0.1.45 (31 December 2023)

#### Improvements

- various bugfixes
- improve solo perform
- launchpad follow mode
- launchpad improve slide visualization
- launchpad color theme
- launchpad circuit dtyle note editor

## v0.1.44 (29 December 2023)

#### Improvements

- add curve improvements [thanks to jackpf](https://github.com/jackpf/performer/blob/master/doc/improvements/shape-improvements.md)
- add midi improvements [thanks to jackpf](https://github.com/jackpf/performer/blob/master/doc/improvements/midi-improvements.md)
- add noise reduction [thanks to jackpf](https://github.com/jackpf/performer/blob/master/doc/improvements/noise-reduction.md)
- add track names
- add quick tie notes feature

## v0.1.43 (27 December 2023)

#### Improvements

- Extend retrigger value. max 8 retriggers per step
- Extend probability steps. min value now is 6.3%
- add negative gate offset feature [thanks to vafu](https://github.com/vafu/performer/tree/vafu/negative-offset)
- add metropolis style sequencer option [thanks to vafu](https://github.com/vafu/performer/tree/vafu/metro-pr)
- double click to toggle gates when editing layers other than gate
- do not reset cv outputs when clock is stopped
- add various curves
- use random seed each access to random gen layer

## v0.1.42 (6 June 2022)

#### Fixes

- Support Launchpad Mk3 with latest firmware update (thanks to Novation support)

## v0.1.41 (18 September 2021)

#### Improvements

- Add support for Launchpad Pro Mk3 (thanks to @jmsole)
- Allow MIDI/CV track to only output velocity (#299)

#### Fixes

- Fix quick edit (#296)
- Fix USB MIDI (thanks to @av500)

## v0.1.40 (9 February 2021)

Note: This release changes the behavior of how note slides are generated. You may need to adjust the _Slide Time_ on existing projects to get a similar feel.

#### Improvements

- Improved behavior of slide voltage output (#243)
- Added _Play Toggle_ and _Record Toggle_ routing targets (#273)
- Retain generator parameters between invocations (#255)
- Allow generator parameters to be initialized to defaults
- Select correct slot when saving an autoloaded project (#266)
- React faster to note events when using MIDI learn
- Filter NPRN messages when using MIDI learn

#### Fixes

- Fixed curve playback cursor when track is muted (#261)
- Fix null pointer dereference in simulator (#284)

## v0.1.39 (26 October 2020)

#### Improvements

- Added support for Launchpad Mini MK3 and Launchpad X (#145)
- Improved performance of sending MIDI over USB

## v0.1.38 (23 October 2020)

#### Fixes

- Fixed MIDI output from monitoring during playback (#216)
- Fixed hanging step monitoring when leaving note sequence edit page

#### Improvements

- Added _MIDI Input_ option to select MIDI input for monitoring/recording (#197)
- Added _Monitor Mode_ option to set MIDI monitoring behavior
- Added double tap to toggle gates on Launchpad controller (#232)

## v0.1.37 (15 October 2020)

#### Fixes

- Output curve track CV when recording (#189, #218)
- Fix duplicate function on note/curve sequence page (#238)
- Jump to first row when switching between user scales
- Fixed printing of route min/max values for certain targets

#### Improvements

- Added _Fill Muted_ option to note tracks (#161)
- Added _Offset_ parameter to curve tracks (#221)
- Allow setting swing on tempo page when holding `PERF` button
- Added inverted loop conditions (#162)
- Improved step shifting to only apply in first/last step range (#196)
- Added 5ms delay to CV/Gate conversion to avoid capturing previous note (#194)
- Allow programming slides/ties using pitch/modulation control when step recording (#228)
- Added _Init Layer_ generator that resets the current layer to its default value (#230)
- Allow holding `SHIFT` for fast editing of route min/max values

## v0.1.36 (29 April 2020)

#### Fixes

- Update routings right after updating each track to allow its CV output to accurately modulate the following tracks (#167)

#### Improvements

- Added fill and mute functions to pattern mode on Launchpad (#173)
- Added mutes to song slots (#178)
- Added step monitoring on curve sequences (#186)
- Added a `hwconfig` to support DAC8568A (in addition to the default DAC8568C)

## v0.1.35 (20 Jan 2020)

#### Fixes

- Fix loading projects from before version 0.1.34 (#168)

## v0.1.34 (19 Jan 2020)

**PLEASE DO NOT USE THIS VERSION, IT CONTAINS A BUG PREVENTING IT FROM READING OLD PROJECT FILES!**

#### Fixes

- Fix inactive sequence when switching track mode (#131)

#### Improvements

- Added _Scale_ and _Root Note_ as routing targets (#166)
- Expanded number of MIDI outputs to 16 (#159)
- Expanded routable tempo range to 1..1000 (#158)
- Generate MIDI output from track monitoring (#148)
- Allow MIDI/CV tracks to consume MIDI events (#155)
- Default MIDI output note event settings with velocity 100
- Indicate active gates of a curve sequence on LEDs

## v0.1.33 (12 Nov 2019)

#### Fixes

- Fixed handling of root note and transposing of note sequences (#147)

#### Improvements

- Add mute mode to curve tracks to allow defining the mute voltage state (#151)
- Increased double tap time by 50% (#144)

## v0.1.32 (9 Oct 2019)

#### Fixes

- Fix _Latch_ and _Sync_ modes on permanent _Performer_ page (#139)

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
