# Fish Feeder Web App Blueprint

## Goal
Build a simple, ocean-themed web dashboard to control the ESP32 Automatic Fish Feeder.

## Requirements
1. **Web Dashboard (`web/index.html`)**:
   - Single-file HTML page with CSS and JavaScript.
   - Ocean blue theme with swimming fish and bubble animations.
   - A big "Feed Now" button for manual feeding.
   - Input boxes to set Morning and Evening feeding times.
   - A Feeding History log section showing timestamps.

2. **ESP32 Code Refactor (`04 main feeder system.ino`)**:
   - Add `#include <WebServer.h>`.
   - Host a local web server on port 80.
   - Serve `web/index.html` on `/`.
   - Handle `/feed` to trigger `dispenseFood()`.
   - Handle `/set-schedule` to save new times.