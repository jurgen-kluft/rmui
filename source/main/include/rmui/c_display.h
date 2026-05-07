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
            u16* m_fb[2];        // RGB565 pixel data for two framebuffers
            u16  m_width;        // width of the framebuffer in pixels
            u16  m_height;       // height of the framebuffer in pixels
            u8*  m_tile_buffer;  // tile buffer for tracking dirty tiles
            u8   m_tile_width;   // width of a tile in pixels (e.g. 16)
            u8   m_tile_height;  // height of a tile in pixels (e.g. 16)
            u8   m_front;        // 0 or 1, indicating which framebuffer is currently being displayed
        };

        void init(display_t* display, u16 width, u16 height, u8 tile_width, u8 tile_height);
        void render(display_t& display, const u8* frame_delta, u32 size);
        void swap(display_t& display);

    }  // namespace nmui
}  // namespace ncore

#endif  // __ARDUINO_REMOTE_UI_CLIENT_H__
