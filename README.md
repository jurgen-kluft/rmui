# Arduino Remote UI Client (rmui)

This is a remote UI client library where the server renders the UI and this client here receives it.

## Features

- Server-side rendering of UI pages
- Client-side update of display from received delta frame data
- Client-side TCP connection to server to receive UI related data
- Client-side sends display HARDWARE details to server to render the UI:
  - Display IC (e.g. CO5300, SH8601Z)
  - Screen resolution (e.g. 320x240, 410x502)
  - Color depth (e.g. 16-bit, e.g. RGB565)
- Frame updates are compressed, so the client side has to decompress the incoming data
- Client-side is the master and request frames from the server
- Client-side can send variables to the server that should be updated in the UI
  - Temperature, Humidity, Pressure, Presence, CO2, CO, etc.

## Constraints

- On the UI, mostly the following UI elements are dynamic:
  - Time and Date
  - Values like Temperature, Humidity, etc.
  - Buttons and their states (e.g. on/off)
  - Graphs and charts (like a Temperature graph)
  - Images (e.g. weather icons, if the weather changes)
  - Text fields (e.g. news headlines, notifications)
- UI should take care in updating many UI elements to prevent inflating the compressed frame size
