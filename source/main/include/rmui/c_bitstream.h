#ifndef __ARDUINO_REMOTE_UI_BITSTREAM_H__
#define __ARDUINO_REMOTE_UI_BITSTREAM_H__
#include "rcore/c_target.h"
#ifdef USE_PRAGMA_ONCE
    #pragma once
#endif

#include "rcore/c_memory.h"

namespace ncore
{
    namespace nmui
    {
        struct bitstream_reader_t
        {
            u8 const *m_stream;
            i32       m_pos;
            i32       m_maxBufferLen;
            i16       m_accuNumBits;
            u32       m_accuRegister;
        };

        // Initialize the reader
        static inline void bsr_init(bitstream_reader_t *bs, u8 const *stream, i32 maxStreamLen)
        {
            bs->m_stream       = stream;
            bs->m_maxBufferLen = maxStreamLen;
            bs->m_pos          = 0;
            bs->m_accuNumBits  = 0;
            bs->m_accuRegister = 0;
        }

        // Ensure at least 24 bits are available in the accumulator
        static inline void bsr_check_accumulator(bitstream_reader_t *bs)
        {
            while (bs->m_accuNumBits <= 24 && bs->m_pos < bs->m_maxBufferLen)
            {
                bs->m_accuRegister |= ((u32)bs->m_stream[bs->m_pos]) << bs->m_accuNumBits;
                bs->m_accuNumBits += 8;
                bs->m_pos++;
            }
        }

        static inline i32 bsr_read_bits(bitstream_reader_t *bs, u8 n)
        {
            bsr_check_accumulator(bs);
            u32 v = bs->m_accuRegister & ((1UL << n) - 1);
            bs->m_accuRegister >>= n;
            bs->m_accuNumBits -= n;
            return (i32)v;
        }

    }  // namespace nmui
}  // namespace ncore

#endif  // __ARDUINO_REMOTE_UI_BITSTREAM_H__