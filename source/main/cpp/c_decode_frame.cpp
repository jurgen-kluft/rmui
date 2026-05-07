#include "rmui/c_decode_frame.h"
#include "rmui/c_srle_stream_reader.h"

namespace ncore
{
    namespace nmui
    {
        void decode_frame(const encoded_frame_t* encoded_frame, u16 const* previous, u16* current)
        {
            u8 const*  p8_rb          = (const u8*)encoded_frame->m_p8_rb;
            u8 const*  p4_rb          = (const u8*)encoded_frame->m_p4_rb;
            u8 const*  p2_rb          = (const u8*)encoded_frame->m_p2_rb;
            u8 const*  selector_rb    = (const u8*)encoded_frame->m_selector_rb;
            u8 const*  line_change_rb = (const u8*)encoded_frame->m_line_change_rb;
            u8 const*  run_change_rb  = (const u8*)encoded_frame->m_run_change_rb;
            u16 const* palette        = (u16*)encoded_frame->m_palette;

            u16 const* p16         = (u16 const*)(encoded_frame + 1);                               // P16 stream starts immediately after the encoded frame header
            u8 const*  p8          = (u8 const*)(p16 + (encoded_frame->m_p16_size / sizeof(u16)));  // P8 stream starts after the P16 stream
            u8 const*  p4          = p8 + encoded_frame->m_p8_encoded_size;                         // P4 stream starts after the P8 stream
            u8 const*  p2          = p4 + encoded_frame->m_p4_encoded_size;                         // P2 stream starts after the P4 stream
            u8 const*  selector    = p2 + encoded_frame->m_p2_encoded_size;                         // Selector stream starts after the P2 stream
            u8 const*  line_change = selector + encoded_frame->m_selector_encoded_size;             // Line change stream starts after the selector stream
            u8 const*  run_change  = line_change + encoded_frame->m_line_change_encoded_size;       // Run change stream starts after the line change stream

            srle_stream_reader_t p8_reader;
            srle_stream_reader_t p4_reader;
            srle_stream_reader_t p2_reader;
            srle_stream_reader_t selector_reader;
            srle_stream_reader_t line_change_reader;
            srle_stream_reader_t run_change_reader;

            // void init_srle_stream_reader(srle_stream_reader_t* r, u8* rb, u8* bitstream, u32 max_units, u8 symbol_bits)
            init(&p8_reader, p8_rb, p8, encoded_frame->m_p8_decoded_units, 8);
            init(&p4_reader, p4_rb, p4, encoded_frame->m_p4_decoded_units, 4);
            init(&p2_reader, p2_rb, p2, encoded_frame->m_p2_decoded_units, 2);
            init(&selector_reader, selector_rb, selector, encoded_frame->m_selector_decoded_units, 2);
            init(&line_change_reader, line_change_rb, line_change, encoded_frame->m_line_change_decoded_units, 1);
            init(&run_change_reader, run_change_rb, run_change, encoded_frame->m_run_change_decoded_units, 1);

            const u16* p16_reader = p16;

            // pseudo code for frame decoder
            // for y < height:
            //    line_change = read_symbol(&line_change_reader);
            //    if (line_change == 1)
            //       num_runs = (width + run-length - 1) / run-length
            //       for run in num_runs:
            //          x_begin = run * run-length
            //          x_end   = min(x_begin + run-length, width)
            //          run_change = read_symbol(&run_change_reader);
            //          if (run_change == 1)
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
        }

    }  // namespace nmui
}  // namespace ncore
