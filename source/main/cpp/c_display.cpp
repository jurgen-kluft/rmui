#include "rmui/c_display.h"
#include "cfenc/c_decoder.h"
#include "rcore/c_system.h"
#include "ccore/c_memory.h"

namespace ncore
{
    namespace nmui
    {
        void init(display_t* display, u16 display_width, u16 display_height, u8 tile_width, u8 tile_height)
        {
            display->m_width  = display_width;
            display->m_height = display_height;
            display->m_front  = 0;

            // use PSRAM to allocate the framebuffers since they are quite large
            // the tile buffer is small enough to fit in regular RAM
            display->m_fb = (u16*)nsystem::alloc_psram(display_width * display_height * sizeof(u16));

            display->m_tiles.m_tile_w     = tile_width;
            display->m_tiles.m_tile_h     = tile_height;
            display->m_tiles.m_tile_count = ((display_width + tile_width - 1) / tile_width) * ((display_height + tile_height - 1) / tile_height);
            display->m_tiles.m_tile_array = (u16*)nsystem::malloc(display->m_tiles.m_tile_count * sizeof(u16));

            // clear tile array
            for (u16 i = 0; i < display->m_tiles.m_tile_count; ++i)
                display->m_tiles.m_tile_array[i] = 0;
        }

        void render_to_display(const display_t& display)
        {
            // Here we render the front buffer to the display, using the tile buffer to optimize rendering by only redrawing dirty tiles.

            const u16 num_h_tiles = (display.m_width + display.m_tiles.m_tile_w - 1) / display.m_tiles.m_tile_w;
            const u16 num_v_tiles = (display.m_height + display.m_tiles.m_tile_h - 1) / display.m_tiles.m_tile_h;

            for (u16 tile_y = 0; tile_y < num_v_tiles; ++tile_y)
            {
                for (u16 tile_x = 0; tile_x < num_h_tiles; ++tile_x)
                {
                    u16 tile_index = tile_y * num_h_tiles + tile_x;
                    if (display.m_tiles.m_tile_array[tile_index] > 0)
                    {
                        // Calculate the pixel coordinates for this tile
                        u16 x_start = tile_x * display.m_tiles.m_tile_w;
                        u16 y_start = tile_y * display.m_tiles.m_tile_h;
                        u16 x_end   = (x_start + display.m_tiles.m_tile_w < display.m_width) ? (x_start + display.m_tiles.m_tile_w) : display.m_width;
                        u16 y_end   = (y_start + display.m_tiles.m_tile_h < display.m_height) ? (y_start + display.m_tiles.m_tile_h) : display.m_height;

                        // TODO ====> Update this tile to the hardware display
                        for (u16 y = y_start; y < y_end; ++y)
                        {
                                u16* span = display.m_fb + (y * display.m_width + x_start);
                                u16* hwdisplay = nullptr;
                                // copy to frame-buffer
                                g_memcpy(hwdisplay, span, sizeof(u16) * (x_end - x_start));
                        }

                        // After updating this tile to the display, we can clear dirty for this tile!
                        display.m_tiles.m_tile_array[tile_index] = 0;
                    }
                }
            }
        }

        // Here we swap the front and back buffers, and clear the tile buffer for the new back buffer.
        void swap(display_t& display)
        {
            display.m_front = 1 - display.m_front;  // Swap between 0 and 1
            render_to_display(display);             // Render the new front buffer to the display
        }

    }  // namespace nmui
}  // namespace ncore
