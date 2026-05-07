

/*

# Receive a frame update

On the ESP32 we have limited resources, so we need to send a frame update in chunks.
We have decided that a data chunk size = 8 KiB, and that our first message will be a
frame header message containing enough information to start receiving the frame data 
in chunks. The frame header message will contain the following information:

- Message ID (4 bytes, 0xF3AB2C1)
- Frame ID (2 bytes)
- Frame width (2 bytes)
- Frame height (2 bytes)
- Total frame data size (4 bytes)
- Number of chunks (2 bytes)

We would like to receive the full frame in a static array on the ESP32, and we will
assume that a full frame update will not exceed 256 KiB in size, which means that we 
can use a static array of 256 KiB to store the frame data.

Between the frame header message and the end-frame message, we will receive a series 
of frame data chunk messages, each containing a chunk of the frame data. 
Each frame data chunk message will contain the following information:

- Message ID (4 bytes, 0xF3ABC8DB (intermediate) or 0xF3ABEF9A (final))
- Frame ID (2 bytes)
- Chunk index (2 bytes)
- Chunk size (2 bytes)
- Chunk data (up to 8 KiB)

The last message will be have the Message ID 0xF3ABEF9A (end-frame message), which will 
indicate that this is the last chunk of the frame data, and that the full frame has been 
received. So an end-frame message will be identical to an intermediate frame data chunk
message except for the Message ID, which will be 0xF3ABEF9A instead of 0xF3ABC8DB.

*/

