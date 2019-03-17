import sys
import os
import testframework as tf

SCREENSHOT_DIR = "manual-screenshots"

os.makedirs(SCREENSHOT_DIR, exist_ok=True)

env = tf.Environment()
c = tf.Controller(env.simulator)

c.screenshotDir(SCREENSHOT_DIR)
c.wait(4000)

project = env.sequencer.model.project

# page-tempo
c.down("clock").wait(100).screenshot("page-tempo").up("clock").wait(100)
c.down("clock").wait(100).down("next").wait(800).screenshot("page-tempo-nudge").up("next").up("clock").wait(100)

# page-clock
c.selectPage("clock").screenshot("page-clock")

# page-overview
c.selectPage("overview").screenshot("page-overview")

# page-pattern
c.selectPage("patt").screenshot("page-pattern")

# page-performer
c.selectPage("perf").screenshot("page-performer")

# page-project
c.selectPage("project").screenshot("page-project")

# page-layout
c.selectPage("layout").screenshot("page-layout-mode")
c.press("f2").screenshot("page-layout-link")
c.press("f3").screenshot("page-layout-gate")
c.press("f4").screenshot("page-layout-cv")
c.press("f1").pressEncoder().rotateEncoder(1).screenshot("page-layout-mode-confirm")

project.setTrackMode(0, tf.sequencer.Track.TrackMode.MidiCv)
project.tracks[0].midiCvTrack.voiceConfig = tf.sequencer.MidiCvTrack.VoiceConfig.PitchVelocity
project.tracks[0].midiCvTrack.voices = 2

project.setCvOutputTrack(0, 0)
project.setCvOutputTrack(1, 0)
project.setCvOutputTrack(2, 0)
project.setCvOutputTrack(3, 0)
c.press("f4").screenshot("page-layout-cv-example")
project.clear()

# page-routing
c.selectPage("routing").screenshot("page-routing")
c.pressEncoder().rotateEncoder(6).screenshot("page-routing-edit")

# page-midi-output
c.selectPage("midiout").screenshot("page-midi-output")

# page-user-scale
c.selectPage("userscale").screenshot("page-user-scale")

# page-monitor
c.selectPage("monitor").screenshot("page-monitor-cv-in")
c.press("f2").screenshot("page-monitor-cv-out")
c.press("f3").midi(0, tf.core.MidiMessage.makeNoteOn(0, 60)).screenshot("page-monitor-midi")
c.press("f4").screenshot("page-monitor-stats")

# page-track
c.selectPage("track").screenshot("page-track")

# page-note-track
c.selectPage("track").screenshot("page-note-track")

# page-note-sequence
c.selectPage("sequence").screenshot("page-note-sequence")

# page-note-steps
c.selectPage("steps").screenshot("page-note-steps")

# page-note-steps-quick-access
c.selectPage("steps").down("page").down("step9").screenshot("page-note-steps-quick-access").up("step9").up("page")

# page-curve-track
project.setTrackMode(0, tf.sequencer.Track.TrackMode.Curve)
c.selectPage("track").screenshot("page-curve-track")

# page-curve-sequence
c.selectPage("sequence").screenshot("page-curve-sequence")

# page-curve-steps
for i in range(16):
    project.tracks[0].curveTrack.sequences[0].steps[i].shape = i
c.selectPage("steps").screenshot("page-curve-steps")

# page-midi-cv-track
project.setTrackMode(0, tf.sequencer.Track.TrackMode.MidiCv)
c.selectPage("track").screenshot("page-midi-cv-track")

# page-song
c.selectPage("song").screenshot("page-song")
c.down("f2").wait(100).press("step1").press("step1").press("step1").press("step2").press("step1").press("step1").press("step1").press("step3").up("f2").wait(100).screenshot("page-song-chain-example")

# page-system
c.selectPage("system").screenshot("page-system-confirm")
c.press("f5").screenshot("page-system-cal")
c.pressEncoder().screenshot("page-system-cal-edit").pressEncoder()
c.press("f4").screenshot("page-system-utils")
c.press("f5").screenshot("page-system-update")

