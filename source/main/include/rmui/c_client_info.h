#ifndef __RMUI_CLIENT_INFO_H__
#define __RMUI_CLIENT_INFO_H__
#include "rcore/c_target.h"
#ifdef USE_PRAGMA_ONCE
    #pragma once
#endif

namespace ncore
{
    namespace nmui
    {
        struct client_info_t
        {
            u16 m_display_format;  // encoded display format (see c_display_format.h)
            u16 m_screen_width;
            u16 m_screen_height;
            u8  m_mac_addr[6];
        };

    }  // namespace nmui
}  // namespace ncore

#endif  // __RMUI_CLIENT_INFO_H__