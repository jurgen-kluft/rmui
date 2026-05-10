# Arduino Remote UI Client (rmui)

This is a remote UI client library where the server renders the UI and this client here receives it.

## ESP32-S3

This library is mainly targetted for the ESP32-S3 microcontroller, mainly because the frame-buffer
needs to be stored in PSRAM and the ESP32-S3 has a large amount of it (8MB). The ESP32-S3 also has a 
powerful enough CPU that can handle the decompression of the incoming frames.

## Features

- Server-side rendering of UI pages
- Client-side is the master and request frames from the server
  - Can request a full frame or just a delta frame (only the changes since the last frame)
    (A full frame has the previous frame set to a particular color filled screen)
- Client-side update of display from received compressed delta frame data
  - Frame updates are compressed, so the client side has to decompress the incoming data
- Client-side TCP connection to server to receive UI related data
- Client-side sends display HARDWARE details to server to render the UI:
  - Display IC (e.g. CO5300, SH8601Z)
  - Screen resolution (e.g. 320x240, 410x502)
  - Color depth (e.g. 16-bit, e.g. RGB565)
- Client-side can send variables to the server that should be updated in the UI
  - Temperature, Humidity, Pressure, Presence, CO2, CO, etc.

## Constraints

- Server side has access to many basic but also more complex information:
  - Time and Date
  - Weather information (e.g. from an API)
  - News headlines (e.g. from an API)
  - Notifications (e.g. from an API or local events)
  - Calendar events (e.g. from an API or local calendar)
  
- On the UI, mostly the following UI elements are dynamic:
  - Time and Date
  - Values like Temperature, Humidity, etc.
  - Buttons and their states (e.g. on/off)
  - Graphs and charts (like a Temperature graph)
  - Images (e.g. weather icons, if the weather changes)
  - Text fields (e.g. news headlines, notifications)
- UI should take care in updating many UI elements to prevent inflating the compressed frame size
