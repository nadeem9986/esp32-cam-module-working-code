# ESP32-CAM Live Stream - Working Code

ESP32-CAM code that actually works, arrived at after going through many versions that didn't. The camera turns on its own Wi-Fi hotspot; connect to it, open the IP address in a browser, and you get the live camera stream. No router or extra hardware needed.

## Demo

_Add a phone screenshot of the live stream in the browser here._

## How it works

1. Copy `credentials.example.h` to `credentials.h` and set your own hotspot name and a strong password (8+ characters), then flash the code to the ESP32-CAM.
2. Power it up - it starts a Wi-Fi hotspot.
3. Connect your phone or laptop to that hotspot.
4. Open the printed IP address in a browser to watch the live stream.

## Hardware

- ESP32-CAM module

## Security note

The hotspot credentials live in `credentials.h`, which is git-ignored so real values never land in the repo. Set your own before flashing - anyone connected to the hotspot can open the stream.

## Status

Working and in active development - more features are being added on top of the streaming base.
