---
title: Build Guide
permalink: /build/
layout: manual
nav: 30
---

# Build Guide

## Introduction

**DISCLAIMER: Building the per\|former sequencer is not recommended for beginners!**

This build guide is intended to give experienced builders all the required information to successfully build the **per\|former** sequencer.

## Sourcing Parts

To build the sequencer you first need to source all the required parts. A complete bill of materials (BOM) can be found here:

[Google Sheet BOM](https://docs.google.com/spreadsheets/d/1XFOsXiIkOcD5iRhrXQCekjhakFLR-p3aiQ2NIJsRs8M/edit?usp=sharing)

To make sourcing parts a bit easier, there is also a BOM at Octopart:

[Octopart BOM](https://octopart.com/bom-tool/ZDJ2VMnB)

The Octopart BOM contains all parts except the jacks, encoder knob and the standoffs and makes it easy to order from the two main distributors [Mouser](https://www.mouser.com) and [Digikey](https://www.digikey.com).

> Note: Due to the current shortage of surface mount capacitors it can be a bit hard to source some values of capacitors. Note that most of these are used as bypass capacitors and should be easily replaceable with a different part of similar specification.

A good source for the remaining parts is [Thonk](https://www.thonk.co.uk) in the UK:

- [PJS008U-3000-0](https://www.thonk.co.uk/shop/radio-music-sd-card-holder-only/) (also available at [Mouser](https://www.mouser.com) for builders outside of Europe)
- [PJ301BM and PJ301CM](https://www.thonk.co.uk/shop/3-5mm-jacks/)
- [Encoder Knobs](https://www.thonk.co.uk/shop/sifam-soft-touch-encoder-knobs/)

## Assembly

To help populating the PCB, its always handy to have access to the schematic and board layout. If you don't have [KiCad](http://kicad-pcb.org) installed to open the board files, you can also access the all relevant files online:

- [Schematics](https://cdn.rawgit.com/westlicht/performer-hardware/master/sequencer.pdf)
- [Front Silkscreen](https://eyrie.io/board/244e2550832a4021a9b4df8c87535577?pours=true&active=layout&layers=m000000000a010000000000000000000000000000000000000000000000000000000000000006&x=162560&y=54367&w=202964&h=117513&flipped=false)
- [Back Silkscreen](https://eyrie.io/board/244e2550832a4021a9b4df8c87535577?pours=true&active=layout&layers=m0000000005010000000000000000000000000000000000000000000000000000000000000006&x=162560&y=54367&w=202964&h=117513&flipped=true)

> Note: The online board viewer can also show all the other layers of the board which is handy to find where traces go and where to measure the different power rails.

### Power Section

It is best to populate the power section first. Check the _Power_ sheet in the schematics to find the required components.

![](images/power-section.jpg)

With the power section in place, it is a good idea to make sure there are no shorts between the **GND**, **+12V** and **-12V** pins on the power connector. With that confirmed, you can connect the board to a power supply to measure all the power rails for proper voltages. These can be measured on any pad on the following nets: **+12V**, **-12V**, **+5V**, **+3V3**, **+3.3VA** and **AREF_-10V**. Use the online board viewer to highlight the nets.

### MCU Section

Populate the STM32 and the components around it found on the _MCU_ sheet in the schematics. This allows for testing the JTAG connection to the STM32 and allow flashing the firmware.

![](images/stm32-section.jpg)

### Other Sections

Populate all other components, except for the jacks, USB connector, SD card holder, encoder and LEDs, as they will need proper alignment with the frontpanel.

![](images/all-sections.jpg)

### Connectors

Populate three of the PJ301BM jacks, put on the frontpanel and tighten the nuts on the jacks. Make sure all three jacks are firmly in place and solder them down.

![](images/three-jacks.jpg)

After that, remove the frontpanel, populate the remaining PJ301BM and the two PJ301CM jacks. Put the frontpanel back on and tighten the nuts on the three previously soldered jacks. This lets you solder in the remaining jacks, making sure they are flush with the PCB. Note that the two PJ301CM jacks are actually lifted off the board a tiny bit. These two are best tightened with nuts before soldering to get the best alignment.

With all the jacks in place, remove the frontpanel again. Populate the USB connector and SD card holder as well as the encoder. Put the frontpanel back on to check alignment. It may be a good idea to plug in a USB cable to ensure alignment of the USB connector. Also make sure that the encoder is flush on the PCB and perfectly perpendicular.

![](images/connectors.jpg)

### LEDs

Remove the frontpanel again and populate all the LEDs. Note that the shortest leg goes into the rectangular pad. With all LEDs in place, put the frontpanel back on and tighten the same three jacks and the spacer. Use tape to have all the LEDs flush to the frontpanel or use some tool to allow them come through the frontpanel just a little bit, whatever your taste is. Solder the middle leg of all LEDs and check alignment before soldering the remaining two legs of all the LEDs.

### OLED Display

Prepare the display by soldering a row of pin headers.

![](images/oled-pin-header.jpg)

You will most probably need to shorten the pin header by a few millimeters to make it fit below the frontpanel. Use 8mm spacers to attach the display to the main PCB. I just used 4 nuts and only two of the 4 supports, which should be sufficient.

![](images/oled-assembly.jpg)

Before putting back the frontpanel (for the last time), add the rectangular switch caps on the 5 switches below the display, because they cannot be put in from above the frontpanel.

![](images/switch-caps-missing.jpg)

### Switch Caps

Finally, put a round switch cap to all of the remaining switches. Note that they need to pressed quite firmly to put into the right spot. You can also put on the knob on the encoder now.

![](images/switch-caps.jpg)

## Flashing Firmware

Flashing the firmware can be done in multiple ways. The easiest way is to download the latest firmware from the [Github Releases](https://github.com/westlicht/performer/releases) page and use the flasher of your choice. Both **bootloader.hex** and **sequencer.hex** need to be downloaded and flashed to run the sequencer. There is also **tester.hex**, a hardware test application that can be used to fully test all hardware components.

With the firmware loaded, you now hopefully have a fully working **per\|former** sequencer.

![](images/final.jpg)


