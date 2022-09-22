# Improvement: Noise Reduction

The OLED screen that the performer uses is inherently noisy. It generates noise on the power bus, and also on its outputs,
meaning any other module that is connected to the performer will pick up this noise (particularly VCOs & filters).

It's not very noticeable, unless you amplify, compress or overdrive the output, but it's definitely there.

The issue with some further information on the original original repository is [here](https://github.com/westlicht/performer/issues/304).

The problem is not entirely solvable by software (since it's a hardware issue), but basically the amount of noise is directly
related to the pixels that are lit up on the OLED screen.

This change provides some settings to reduce the noise as much as possible (I think I achieved around 20dB reduction).

## Guide

1. Open up the **System** page (**PAGE** + **SYSTEM**)
2. Press **SETTINGS** (**F5**)

## Settings

| Setting      	| Description                                                                                                                                                                                                                                                                                                                                                                       	|
|--------------	|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------	|
| Brightness   	| Reduces the overall brightness of the display. Lower brightness will reduce noise.                                                                                                                                                                                                                                                                                                	|
| Screensaver  	| Turns the screen off after the specified time, or "off" to disable. When the screen is off, the noise is completely gone. This is useful in a studio setting, when you'd like to record something completely noise-free.                                                                                                                                                          	|
| Wake Mode    	| Determines when the screen should "wake up" from the screensaver. "always" will wake the screen on any key press. "required" allows the screen to stay off when performing tasks that don't really require the screen, e.g. modifying a sequence (since the LEDs describe the current sequence). This is also a pseudo "screen-less" mode for the sequencer, which can be nice :) 	|
| Dim Sequence 	| The sequence page is particularly noisy if a lot of steps are enabled from the steps currently displayed on-screen. With dim sequence on, the squares are slightly darker than normal, which reduces noise on the sequence page substantially.                                                                                                                                    	|

One change that is not added as a setting is changing the footer UI elements (the text relating the the **F** keys)
so that the active function is bold instead of highlighted. This reduces noise drastically.