#include "rmui/c_display.h"
#include "cfenc/c_decoder.h"
#include "rcore/c_system.h"
#include "ccore/c_memory.h"

namespace ncore
{
    namespace nmui
    {
        void init(display_t* display, u16 display_width, u16 display_height)
        {
            display->m_width  = display_width;
            display->m_height = display_height;

            // use PSRAM to allocate the framebuffers since they are quite large
            // the tile buffer is small enough to fit in regular RAM
            display->m_fb = (u16*)nsystem::alloc_psram(display_width * display_height * sizeof(u16));
        }

        static inline bool s_is_tile_dirty(const u8* tile_line, u16 tile_x)
        {
            const u16 byte_index = tile_x >> 3;
            const u16 bit_index = tile_x & 0x7;
            return tile_line[byte_index] & (1 << bit_index); 
        }

        void render(display_t& display, const tiles_t& tiles)
        {
            // Here we render the front buffer to the display, using the tile buffer to optimize rendering by only redrawing dirty tiles.

            const u16 num_h_tiles = tiles.m_tile_count_h;
            const u16 num_v_tiles = tiles.m_tile_count_v;
           
            const u8* tile_line = tiles.m_tiles;
            for (u16 tile_y = 0; tile_y < num_v_tiles; ++tile_y)
            {
                for (u16 tile_x = 0; tile_x < num_h_tiles; ++tile_x)
                {
                    if (s_is_tile_dirty(tile_line, tile_x))
                    {
                        // Calculate the pixel coordinates for this tile
                        const u16 x_start = tile_x * tiles.m_tile_w;
                        const u16 y_start = tile_y * tiles.m_tile_h;
                        const u16 x_end   = (x_start + tiles.m_tile_w < display.m_width) ? (x_start + tiles.m_tile_w) : display.m_width;
                        const u16 y_end   = (y_start + tiles.m_tile_h < display.m_height) ? (y_start + tiles.m_tile_h) : display.m_height;

                        // TODO ====> Update this tile to the hardware display
                        for (u16 y = y_start; y < y_end; ++y)
                        {
                                u16* span = display.m_fb + (y * display.m_width + x_start);
                                u16* hwdisplay = nullptr;
                                // copy to frame-buffer
                                g_memcpy(hwdisplay, span, sizeof(u16) * (x_end - x_start));
                        }
                    }
                }

                tile_line += tiles.m_tile_stride;
            }
        }

    }  // namespace nmui
}  // namespace ncore
