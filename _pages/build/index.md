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

## Populating PCB

To help populating the PCB, its always handy to have access to the schematic and board layout. If you don't have [KiCad](http://kicad-pcb.org) installed to open the board files, you can also access the all relevant files online:

- [Schematics](https://cdn.rawgit.com/westlicht/performer-hardware/master/sequencer.pdf)
- [Front Silkscreen](https://eyrie.io/board/244e2550832a4021a9b4df8c87535577?pours=true&active=layout&layers=m000000000a010000000000000000000000000000000000000000000000000000000000000006&x=162560&y=54367&w=202964&h=117513&flipped=false)
- [Back Silkscreen](https://eyrie.io/board/244e2550832a4021a9b4df8c87535577?pours=true&active=layout&layers=m0000000005010000000000000000000000000000000000000000000000000000000000000006&x=162560&y=54367&w=202964&h=117513&flipped=true)

> Note: The online board viewer can also show all the other layers of the board which is handy to find where traces go and where to measure the different power rails.

It's generally a good idea to populate the power section first. Check the POWER sheet in the schematics to find the required components.

TODO: image of assembled power section

Next, measure all the power rails for proper voltages. These can be measured on any pad on the following nets: **+12V**, **-12V**, **+5V**, **+3V3**, **+3.3VA** and **AREF_-10V**. Use the online board viewer to highlight the nets.

TODO: image of measurement points

Next, populate the STM32 and the components around it found on the MCU sheet in the schematics. This allows for testing the JTAG connection to the STM32 and allow flashing the firmware.

TODO: image of assembled STM32 section

Next, populate all other components, except for the OLED display, jacks, USB connector, SD card holder and LEDs, as they will need proper alignment with the frontpanel.

TODO: image of assembled board

## Assembly

TODO: image of crimping OLED pin header

TODO: image of jack alignment

## Flashing Firmware

Flashing the firmware can be done in multiple ways. The easiest way is to download the latest firmware from the [Github Releases](https://github.com/westlicht/performer/releases) page and use the flasher of your choice. Both `bootloader.hex` and `sequencer.hex` need to be downloaded and flashed to run the sequencer. There is also a hardware testing application that can be used to fully test the hardware components.


