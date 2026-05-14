# 1-Bit Monochrome OLEDs (e.g., SSD1306, SH1106)
  
  Color Depth:        1 bit per pixel (On/Off).
  Data Packing:       8 pixels packed into 1 byte.
  Page Architecture:  Displays split horizontally into 8-pixel-high segments called "Pages".
  Pixel Mapping:      Horizontal bytes where Bit 0 represents the top pixel and Bit 7 represents the bottom pixel of the page row.
  Scan Pattern:       Left-to-right through a page, then dropping down to the next page.
  ESP32-S3 Impact:    Extremely low memory footprint; fits easily in internal SRAM.

# Gray-Scale OLEDs/LCDs (e.g., SSD1327)

  Color Depth:        4 bits per pixel (16 levels of gray).
  Data Packing:       2 pixels packed into 1 byte.
  Pixel Mapping:      High nibble contains pixel N; low nibble contains pixel N+1.
  Scan Pattern:       Linear progression, left-to-right, top-to-bottom.

# E-Paper / E-Ink Displays (e.g., Waveshare, Good Display)
  
  Color Depth:        1 bit (Black/White) or 2 bits (Black/White/Red/Yellow).
  Data Packing:       8 pixels per byte for monochrome layouts.
  Dual-Buffer:        Requires two distinct frame buffers (Current Frame vs. New Frame) to calculate differential screen updates.
  Scan Pattern:       Linear layout, but often requires bit-inversion (0 for Black, 1 for White).

# RGB888 / Packed 24-Bit (High-End SPI/RGB Panels)
  
  Color Depth:        24 bits per pixel.
  Data Packing:       3 bytes per pixel (Red byte, Green byte, Blue byte).
  Byte Ordering:      Typically ordered as BGR or RGB depending on register configuration.
  ESP32-S3 Impact:    Demands massive bandwidth; usually requires the ESP32-S3 Octal PSRAM.

# RGB565 A/B Swapped (Standard LCDs over SPI/8-bit Parallel)
  
  Color Depth:        16 bits per pixel (5 Red, 6 Green, 5 Blue).
  Byte Endianness:    High-byte and low-byte are inverted during SPI transmissions.
  Data Packing:       Byte 1 contains G2-G0 + B4-B0; Byte 2 contains R4-R0 + G5-G3.
  ESP32-S3 Impact:    Handled natively by the ESP32-S3 LCD peripheral using DMA byte-swapping features.

# Sharp Memory LCDs (e.g., LS027B7DH01)
  
  Color Depth:        1 bit per pixel.
  Line-Addressing:    Every horizontal line buffer requires an extra leading 8-bit line-address byte.
  Trailer Padding:    Every line requires a trailing 8-bit dummy byte for dummy clock cycles.
  Scan Pattern:       Left-to-right linear, punctuated by control bytes every row.
