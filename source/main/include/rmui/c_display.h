#ifndef __ARDUINO_REMOTE_UI_CLIENT_H__
#define __ARDUINO_REMOTE_UI_CLIENT_H__
#include "rcore/c_target.h"
#ifdef USE_PRAGMA_ONCE
    #pragma once
#endif

namespace ncore
{
    namespace nmui
    {
        struct display_t
        {
            u16* m_fb[2];      // RGB565 pixel data for two framebuffers
            u16  m_width;      // width of the framebuffer in pixels
            u16  m_height;     // height of the framebuffer in pixels
            u8   m_front;      // 0 or 1, indicating which framebuffer is currently being displayed
            u8   m_reserved1;  // reserved for future use, should be set to 0
            u16  m_reserved2;  // reserved for future use, should be set to 0

            struct tiles_t
            {
                u16  m_tile_w;      // tile width in pixels
                u16  m_tile_h;      // tile height in pixels
                u16  m_tile_count;  // total number of tiles in the array
                u16  m_reserverd;   // reserved for future use, should be set to 0
                u16* m_tile_array;  // array[tile count horizontal, tile count vertical]
            };
            tiles_t m_tiles;  // tile information for the display
        };

        void init(display_t* display, u16 display_width, u16 display_height, u8 tile_width, u8 tile_height);
        void render(display_t& display, const u8* frame_delta, u32 size);
        void swap(display_t& display);

    }  // namespace nmui
}  // namespace ncore

#endif  // __ARDUINO_REMOTE_UI_CLIENT_H__
