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
        struct encoded_frame_t
        {
            u16 m_magic;       // 'FE' in ASCII (0x4645), used to identify the encoded frame data
            u16 m_width;       // Width of the image
            u16 m_height;      // Height of the image
            u16 m_run_length;  // Run length for run change stream

            u8  m_p8_rb[256];         // SRLE run-bits for P8 stream
            u8  m_p4_rb[16];          // SRLE run-bits for P4 stream
            u8  m_p2_rb[4];           // SRLE run-bits for P2 stream
            u8  m_selector_rb[4];     // SRLE run-bits for each symbol
            u8  m_line_change_rb[2];  // SRLE run-bits for line change stream
            u8  m_run_change_rb[2];   // SRLE run-bits for run change stream
            u16 m_palette[276];       // RGB565 palette (4 colors for P2, 16 colors for P4, 256 colors for P8)

            u32 m_p16_size;                  // size of the P16 stream in bytes
            u32 m_p8_encoded_size;           // size of the encoded P8 stream in bytes
            u32 m_p4_encoded_size;           // size of the encoded P4 stream in
            u32 m_p2_encoded_size;           // size of the encoded P2 stream in bytes
            u32 m_selector_encoded_size;     // size of the encoded selector stream in bytes
            u32 m_line_change_encoded_size;  // size of the encoded line change stream in bytes
            u32 m_run_change_encoded_size;   // size of the encoded run change stream in bytes

            u32 m_p16_units;                  // size of the P16 stream in units (unit = 16 bits)
            u32 m_p8_decoded_units;           // size of the decoded P8 stream in units (unit = 8 bits)
            u32 m_p4_decoded_units;           // size of the decoded P4 stream in units (unit = 4 bits)
            u32 m_p2_decoded_units;           // size of the decoded P2 stream in units (unit = 2 bits)
            u32 m_selector_decoded_units;     // size of the decoded selector stream in units (unit = 2 bits)
            u32 m_line_change_decoded_units;  // size of the decoded line change stream in units (unit = 2 bits)
            u32 m_run_change_decoded_units;   // size of the decoded run change stream in units (unit = 1 bit)
        };

        void decode_frame(const encoded_frame_t* encoded_frame, u16 const* previous, u16* current);

    }  // namespace nmui
}  // namespace ncore

#endif  // __ARDUINO_REMOTE_UI_DECODE_FRAME_H__