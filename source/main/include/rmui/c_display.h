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
            u16* m_fb;      // RGB565 pixel data for the framebuffer
            u16  m_width;   // width of the framebuffer in pixels
            u16  m_height;  // height of the framebuffer in pixels
        };

        struct tiles_t
        {
            const u8* m_tiles;         // 2D array of tile dirty bits
            u8        m_tile_w;        // tile width in pixels
            u8        m_tile_h;        // tile height in pixels
            u8        m_tile_stride;   // number of bytes per row in the tile array
            u8        m_tile_count_h;  // number of horizontal tiles
            u8        m_tile_count_v;  // number of vertical tiles
        };

        void init(display_t* display, u16 display_width, u16 display_height);
        void render(display_t& display, const tiles_t& tiles);

    }  // namespace nmui
}  // namespace ncore

#endif  // __ARDUINO_REMOTE_UI_CLIENT_H__
