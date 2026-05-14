# TCP Receive Task on Core 0

To maximize the ESP32-S3's dual-core architecture, you should pin the TCP Receive Task to Core 0 (PRO_CPU), which is where the Wi-Fi and TCP/IP stack (LwIP) natively operate. This prevents cross-core context switching and leaves Core 1 (APP_CPU) entirely free for your SRLEN decoding and PSRAM patching.

Pro-tip: Set the Task Priority of the decoder_task on Core 1 slightly higher than the tcp_parser on Core 0. You want the CPU to prioritize putting pixels on the glass over receiving more data.

## Core 0: The "Stream Feeder" Task

This task acts as the entry point for your remote UI data. It receives chunks from the Go server, identifies the Frame Header, and populates your bsr_add_external_data buffers.

```C++
#include <WiFi.h>
#include <WiFiClient.h>

// Global bitstream readers (allocated in SRAM for speed)
bitstream_reader_t main_reader;
uint8_t* sram_buffer; // e.g., 16KB-32KB in Internal SRAM

void tcp_receive_task(void *pvParameters) {
    WiFiClient client;
    const char* host = "192.168.1.100";
    uint16_t port = 8080;

    while (1) {
        if (!client.connected()) {
            if (client.connect(host, port)) {
                client.setNoDelay(true); // Disable Nagle for instant UI updates
                Serial.println("Connected to UI Server");
            } else {
                vTaskDelay(pdMS_TO_TICKS(1000));
                continue;
            }
        }

        // 1. Wait for frame data
        int available = client.available();
        if (available > 0) {
            uint8_t temp_chunk[1460]; // MTU-sized chunk in SRAM
            int bytesRead = client.read(temp_chunk, sizeof(temp_chunk));
            
            // 2. Feed the "Sliding Window" BitStreamReader
            // Use a mutex if Core 1 is actively reading this specific buffer
            bsr_add_external_data(&main_reader, temp_chunk, bytesRead);
            
            // 3. Notify Core 1 that new data is available
            xTaskNotifyGive(decoder_task_handle);
        } else {
            vTaskDelay(1); // Yield to let LwIP process the radio
        }
    }
}
```

## Core 1: The "Decoder" Task

Pinning this to Core 1 ensures your read_symbol and selector_jump_table calls are never interrupted by Wi-Fi interrupts.

```C++
void decoder_task(void *pvParameters) {
    while (1) {
        // Wait for Core 0 to signal that data is ready
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (bsr_has_enough_for_frame(&main_reader)) {
            // Your Tile-Zig-Zag Decoding Logic Here
            decode_and_patch_frame();
            
            // Trigger 8080-16 Flush
            flush_dirty_tiles();
        }
    }
}
```

## Critical Task Initialization

Use xTaskCreatePinnedToCore in your setup() to lock these roles.

```C++
void setup() {
    // ... [Your XL9555 and LCD Init Code] ...

    // Core 0: Protocol & Networking (PRO_CPU)
    xTaskCreatePinnedToCore(
        tcp_receive_task, "TCP_Feeder", 8192, NULL, 5, NULL, 0
    );

    // Core 1: Application & Graphics (APP_CPU)
    xTaskCreatePinnedToCore(
        decoder_task, "UI_Decoder", 8192, NULL, 5, &decoder_task_handle, 1
    );
}
```

## Key Performance Advantages:

1. Zero-Latency Pipelining: As Core 0 is filling the tail of your SRAM buffer with new TCP chunks, Core 1 is already decompressing the head of the buffer.
2. Shared Memory Performance: On the S3, Core 0 and Core 1 can access different 32KB SRAM banks simultaneously at full speed. By keeping your bitstream buffer in SRAM, you avoid the slower PSRAM bus during the "Feed" phase.
3. Interrupt Handling: By keeping Wi-Fi on Core 0, you ensure that high-frequency network interrupts never "steal" cycles from your time-sensitive 8080-16 parallel bus writes on Core 1.