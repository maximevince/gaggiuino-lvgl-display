# ESP32-S3 Gaggiuino Nextion Replacement

This project aims to replace the Nextion display on a Gaggiuino Gen2 with a more flexible and open-source solution using LVGL (Light and Versatile Graphics Library) on a generic display.
The reason it's using Gaggiuino Gen2 is because this is the last open source version of the project available.

## Project Overview

The Gaggiuino is a community-driven project that adds high-end features to Gaggia Classic espresso machines. This project implements a drop-in replacement for the Nextion display that:

- Uses LVGL library for graphics rendering
- Implements the Nextion protocol to maintain compatibility
- Works with generic displays instead of proprietary Nextion hardware
- Maintains the same functionality as the original Nextion display

## Features

- Nextion protocol compatibility layer
- LVGL-based graphics rendering
- Support for generic displays
- Drop-in replacement for existing Gaggiuino Gen2 setups
- Aims to support all the features of the Gen2 Nextion LCD interface 

## Status

🚧 **Work in Progress**

This project is currently under active development. While it aims to be a complete replacement for the Nextion display, many features may still be in development or need refinement.

## Hardware Requirements

- ESP32-S3 microcontroller
- Guition ESP32-4848S040 display
- Gaggiuino Gen2 hardware
- Gaggia Classic espresso machine

## Software Dependencies

- LVGL library
- ESP32 Arduino core
- Additional dependencies to be documented

## Contributing

Contributions are welcome! This is an open-source project aimed at providing an alternative to proprietary display solutions for the Gaggiuino community.

## License

This project is licensed under the GNU General Public License v3.0 (GPL-3.0) - see the LICENSE file for details.

This license ensures that the software remains free and open-source, allowing users to use, modify, and distribute the code while ensuring that derivative works are also shared under the same terms.

## Acknowledgments

- Original Gaggiuino project
- LVGL community
- Nextion protocol documentation
- Gaggiuino community
