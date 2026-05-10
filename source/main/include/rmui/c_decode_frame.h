#ifndef __ARDUINO_REMOTE_UI_DECODE_FRAME_H__
#define __ARDUINO_REMOTE_UI_DECODE_FRAME_H__
#include "rcore/c_target.h"
#ifdef USE_PRAGMA_ONCE
    #pragma once
#endif

namespace ncore
{
    namespace nmui
    {
        // Encoded Frame Header Structure
        struct encoded_frame_hdr_t
        {
            u16 m_magic;       // 'FE' in ASCII (0x4645), used to identify the encoded frame data
            u16 m_width;       // Width of the image
            u16 m_height;      // Height of the image
            u16 m_run_length;  // Run length for run change stream

            u8  m_p8_rb[256];         // SRLE run-bits for P8 stream
            u8  m_p4_rb[16];          // SRLE run-bits for P4 stream
            u8  m_p2_rb[4];           // SRLE run-bits for P2 stream
            u8  m_selector_rb[4];     // SRLE run-bits for each symbol
            u8  m_run_change_rb[2];   // SRLE run-bits for run change stream
            u8  m_tile_change_rb[2];  // SRLE run-bits for tile change stream
            u8  m_line_change_rb[2];  // SRLE run-bits for line change stream
            u16 m_palette[276];       // RGB565 palette (4 colors for P2, 16 colors for P4, 256 colors for P8)

            u16 m_tile_change_encoded_size;  // size of the encoded tile change stream in bytes
            u16 m_line_change_encoded_size;  // size of the encoded line change stream in bytes
            u16 m_tile_change_decoded_units;  // size of the decoded tile change stream in units (unit = 1 bit)
            u16 m_line_change_decoded_units;  // size of the decoded line change stream in units (unit = 1 bit)
        };

        void decode_frame(const encoded_frame_hdr_t* encoded_frame, u16* fbuffer);

    }  // namespace nmui
}  // namespace ncore

#endif  // __ARDUINO_REMOTE_UI_DECODE_FRAME_H__