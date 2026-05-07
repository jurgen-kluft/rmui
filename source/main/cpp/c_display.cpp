#include "rmui/c_display.h"
#include "rmui/c_decode_frame.h"
#include "rcore/c_system.h"

namespace ncore
{
    namespace nmui
    {
        void init(display_t* display, u16 width, u16 height, u8 tile_width, u8 tile_height)
        {
            display->m_width       = width;
            display->m_height      = height;
            display->m_tile_width  = tile_width;
            display->m_tile_height = tile_height;
            display->m_front       = 0;

            // use PSRAM to allocate the framebuffers since they are quite large
            // the tile buffer is small enough to fit in regular RAM
            display->m_fb[0] = (u16*)nsystem::alloc_psram(width * height * sizeof(u16));
            display->m_fb[1] = (u16*)nsystem::alloc_psram(width * height * sizeof(u16));

            display->m_tile_buffer = (u8*)nsystem::malloc(((width + tile_width - 1) / tile_width) * ((height + tile_height - 1) / tile_height));
        }

        void render(display_t& display, const u8* frame_delta, u32 size)
        {
            // Here we uncompress the frame delta and update the back buffer accordingly,
            // and mark the corresponding tiles as dirty in the tile buffer.
            // The front buffer is our previous frame buffer, and the back buffer is the
            // one we are building for the new frame.
            encoded_frame_t* encoded = (encoded_frame_t*)frame_delta;
            if (encoded->m_magic != 0x4645)  // 'FE' in ASCII
                return;                      // invalid frame data

            u16* previous_fb = display.m_fb[display.m_front];
            u16* current_fb  = display.m_fb[1 - display.m_front];

            decode_frame(encoded, previous_fb, current_fb);
        }

        void render_to_display(const display_t& display)
        {
            // Here we render the front buffer to the display, using the tile buffer to optimize rendering by only redrawing dirty tiles.
        }

        void swap(display_t& display)
        {
            // Placeholder for swap logic
            // Here we swap the front and back buffers, and clear the tile buffer for the new back buffer.

            display.m_front = 1 - display.m_front;  // Swap between 0 and 1

            render_to_display(display);  // Render the new front buffer to the display
        }

    }  // namespace nmui
}  // namespace ncore
