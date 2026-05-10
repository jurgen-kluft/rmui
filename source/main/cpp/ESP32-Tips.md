# Tips

    On the ESP32-S3, make sure your PSRAM psram_buffer is allocated with MALLOC_CAP_SPIRAM and MALLOC_CAP_8BIT to ensure it is cacheable.