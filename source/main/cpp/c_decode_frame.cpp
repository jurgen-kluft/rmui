#include "rmui/c_decode_frame.h"
#include "rmui/c_srle_stream_reader.h"

namespace ncore
{
    namespace nmui
    {
#define TILE_WIDTH  16
#define TILE_HEIGHT 16
#define TILE_PIXELS (TILE_WIDTH * TILE_HEIGHT)

        // For ESP32-S3 we will declare some static arrays that should go into SRAM
        static u16 s_p16_buffer[TILE_PIXELS];                      // buffer for decoded P16 data for one tile (16 bits per pixel)
        static u8  s_p8_bs_buffer[TILE_PIXELS * 2];                // buffer for decoded P8 bitstream data for one tile (worst case: all pixels are 13(8+5) bits)
        static u8  s_p4_bs_buffer[TILE_PIXELS + TILE_PIXELS / 8];  // buffer for decoded P4 bitstream data for one tile (worst case: all pixels are 9(4+5) bits)
        static u8  s_p2_bs_buffer[TILE_PIXELS];                    // buffer for decoded P2 bitstream data for one tile (worst case: all pixels are 7(2+5) bits)
        static u8  s_selector_bs_buffer[TILE_PIXELS];              // buffer for decoded selector bitstream data for one tile (2 bits per pixel)
        static u8  s_run_change_bs_buffer[TILE_PIXELS];            // buffer for decoded line change bitstream data for one tile (worst case: all lines changed, 1 bit per line)
        static u8  s_line_change_decoded_buffer[1024 / 8];         // buffer for decoded line change (max frame-buffer height = 1024, 1 bit per line)
        static u8  s_tile_change_decoded_buffer[(64 * 64) / 8];    // buffer for decoded tile change (max frame-buffer size = 1024x1024)
        static u8  s_run_change_decoded_buffer[32];                // buffer for decoded run change (max tile-height bits)
        static u16 s_tile_line_buffer[TILE_WIDTH];                 // temporary buffer for one line of a tile during decoding, used to minimize writes to PSRAM

        struct stream_context_t
        {
            u16*                 m_p16_reader;
            u32                  m_p16_pos;
            bitstream_reader_t   m_p8_reader_bs;
            bitstream_reader_t   m_p4_reader_bs;
            bitstream_reader_t   m_p2_reader_bs;
            bitstream_reader_t   m_selector_reader_bs;
            bitstream_reader_t   m_run_change_reader_bs;
            srlen_streamreader_t m_p8_reader;
            srlen_streamreader_t m_p4_reader;
            srlen_streamreader_t m_p2_reader;
            srlen_streamreader_t m_selector_reader;
            srlen_streamreader_t m_run_change_reader;
        };

        static stream_context_t s_context;

        void decode_frame(const encoded_frame_hdr_t* hdr, u16 const* previous, u16* current)
        {
            u8 const*  p8_rb          = (const u8*)hdr->m_p8_rb;
            u8 const*  p4_rb          = (const u8*)hdr->m_p4_rb;
            u8 const*  p2_rb          = (const u8*)hdr->m_p2_rb;
            u8 const*  selector_rb    = (const u8*)hdr->m_selector_rb;
            u8 const*  run_change_rb  = (const u8*)hdr->m_run_change_rb;
            u8 const*  tile_change_rb = (const u8*)hdr->m_tile_change_rb;
            u8 const*  line_change_rb = (const u8*)hdr->m_line_change_rb;
            u16 const* palette        = (u16*)hdr->m_palette;

            u8 const* line_change = (u8 const*)(hdr + 1);                           // Line change stream starts immediately after the header
            u8 const* tile_change = line_change + hdr->m_line_change_encoded_size;  // Tile change stream starts immediately after the line change stream

            decode(line_change, hdr->m_line_change_encoded_size, line_change_rb, 1, s_line_change_decoded_buffer, hdr->m_line_change_decoded_units);
            decode(tile_change, hdr->m_tile_change_encoded_size, tile_change_rb, 1, s_tile_change_decoded_buffer, hdr->m_tile_change_decoded_units);

            // PSEUDO code for frame decoder (line version)
            // receive header, decode tile and line change streams
            // when receiving a line packet:
            //    process_line_data(); // init line-index, p2, p4, p8, p16, selector, and run_change stream readers
            //    while at_end(&run_change_reader) == false:
            //       if read_symbol(&run_change_reader) == 1:
            //          for x in tile_width:
            //             selector = read_symbol(&selector_reader);
            //             color = selector_jump_table[selector](context);
            //             write color to temporary tile line buffer at (x)
            //           memcpy temporary tile line buffer to current frame in PSRAM at (tile_x * tile_width, line-index)
            // when receiving end of frame:
            //    for y in num_tiles_y:
            //       for x in num_tiles_x:
            //           scan tiles array at [x,y] skip unchanged tiles and merge any contiguous changed tiles
            //           display::draw-bitmap with this merged tile
            //           x += num merged tiles
            //

            // PSRAM frame-buffer should be allocated with an alignment of 32 bytes

            // Make sure the functions decode_P2, decode_P4, etc., are marked with IRAM_ATTR. This forces the compiler to keep the code in the
            // S3's Instruction RAM, so it never has to fetch from slow Flash memory during the inner loop.
        }

    }  // namespace nmui
}  // namespace ncore

// PSEUDO code for frame decoder (line by line version)
// for y < height:
//    line_change = read_symbol(&line_change_reader);
//    if (line_change == 1)
//       num_runs = (width + run-length - 1) / run-length
//       for run in num_runs:
//          x_begin = run * run-length
//          x_end   = min(x_begin + run-length, width)
//          run_change = read_symbol(&run_change_reader);
//          if (run_change == 1)
//             if tile counter at tile[x_begin / tile_width, y / tile_height] == 0: increment dirty count and set it to 1
//             else: increment dirty tile counter at tile[x_begin / tile_width, y / tile_height]
//             run_length = x_end - x_begin;
//             for r in run_length:
//                 selector = read_symbol(&selector_reader);
//                 switch selector:
//                    case P2: color = palette[read_symbol(&p2_reader)]; break;
//                    case P4: color = palette[4 + read_symbol(&p4_reader)]; break;
//                    case P8: color = palette[20 + read_symbol(&p8_reader)]; break;
//                    case P16: color = *p16_reader++; break;
//                 write color to current frame at (x_begin + r, y)
//          else
//             copy current run[x_begin,x_end) of previous frame to run[x_begin,x_end) of current frame
//    else
//       copy the current line from the previous frame to the current frame