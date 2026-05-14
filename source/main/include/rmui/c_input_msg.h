#ifndef __RMUI_INPUT_MSGS_H__
#define __RMUI_INPUT_MSGS_H__
#include "rcore/c_target.h"
#ifdef USE_PRAGMA_ONCE
    #pragma once
#endif

namespace ncore
{
    namespace nmui
    {
        enum MsgType : u8
        {
            MessageTypeFrameUpdate = 0x01,
            MessageTypeInputEvent = 0x02
        };

        // - Frame update message: [MessageTypeFrameUpdate(byte), flags(byte)]
        static inline u8* write_frame_update_msg(u8* buffer, u8 flags)
        {
            buffer[0] = MessageTypeFrameUpdate;
            buffer[1] = flags;
            return buffer + 2;
        }

        enum InputType : u8
        {
            InputTypeTouch  = 0x01,
            InputTypeSwipe  = 0x02,
            InputTypeButton = 0x03,
            InputTypeRotary = 0x04
        };

        // - Touch event: [MessageTypeInputEvent(byte), InputTypeTouch(byte), x(int16), y(int16)]
        static inline u8* write_touch_event(u8* buffer, s16 x, s16 y)
        {
            buffer[0] = MessageTypeInputEvent;
            buffer[1] = InputTypeTouch;
            buffer[2] = (u8)(x & 0xFF);
            buffer[3] = (u8)((x >> 8) & 0xFF);
            buffer[4] = (u8)(y & 0xFF);
            buffer[5] = (u8)((y >> 8) & 0xFF);
            return buffer + 6;
        }

        // - Swipe event: [MessageTypeInputEvent(byte), InputTypeSwipe(byte), direction(int8), distance(int16)]
        static inline u8* write_swipe_event(u8* buffer, s8 direction, s16 distance)
        {
            buffer[0] = MessageTypeInputEvent;
            buffer[1] = InputTypeSwipe;
            buffer[2] = (u8)direction;
            buffer[3] = (u8)(distance & 0xFF);
            buffer[4] = (u8)((distance >> 8) & 0xFF);
            return buffer + 5;
        }

        // - Button event: [MessageTypeInputEvent(byte), InputTypeButton(byte), buttonId(int16), state(int16)]
        static inline u8* write_button_event(u8* buffer, s16 buttonId, s16 state)
        {
            buffer[0] = MessageTypeInputEvent;
            buffer[1] = InputTypeButton;
            buffer[2] = (u8)(buttonId & 0xFF);
            buffer[3] = (u8)((buttonId >> 8) & 0xFF);
            buffer[4] = (u8)(state & 0xFF);
            buffer[5] = (u8)((state >> 8) & 0xFF);
            return buffer + 6;
        }

        // - Rotary event: [MessageTypeInputEvent(byte), InputTypeRotary(byte), rotation(int32)]
        static inline u8* write_rotary_event(u8* buffer, s32 rotation)
        {
            buffer[0] = MessageTypeInputEvent;
            buffer[1] = InputTypeRotary;
            buffer[2] = (u8)(rotation & 0xFF);
            buffer[3] = (u8)((rotation >> 8) & 0xFF);
            buffer[4] = (u8)((rotation >> 16) & 0xFF);
            buffer[5] = (u8)((rotation >> 24) & 0xFF);
            return buffer + 6;
        }

    }  // namespace nmui
}  // namespace ncore

#endif  // __RMUI_INPUT_MSGS_H__