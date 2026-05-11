#ifndef __ARDUINO_REMOTE_UI_NSRLE_STREAM_H__
#define __ARDUINO_REMOTE_UI_NSRLE_STREAM_H__
#include "rcore/c_target.h"
#ifdef USE_PRAGMA_ONCE
    #pragma once
#endif

#include "rcore/c_memory.h"

namespace ncore
{
    namespace nmui
    {
        struct nsrle_decoder_t
        {
            u8 const *m_stream;
            u32       m_pos;
            u32       m_accuRegister;
            u8        m_accuNumBits;
            u8        m_symbol_run;
            u8        m_symbol;
            u8        m_reserved0;
        };

        // Initialize the nsrle decoder
        static inline void nsrle_init(nsrle_decoder_t *bs, u8 const *stream)
        {
            bs->m_stream       = stream;
            bs->m_pos          = 0;
            bs->m_accuNumBits  = 0;
            bs->m_accuRegister = 0;
            bs->m_symbol_run   = 0;
            bs->m_symbol       = 0;
        }

        // Ensure at least 16 bits are available in the accumulator
        static inline void nsrle_check_accumulator(nsrle_decoder_t *bs)
        {
            if (bs->m_accuNumBits < 16)
            {
                bs->m_accuRegister |= ((u32)bs->m_stream[bs->m_pos]) << bs->m_accuNumBits;
                bs->m_accuNumBits += 8;
                bs->m_pos++;

                bs->m_accuRegister |= ((u32)bs->m_stream[bs->m_pos]) << bs->m_accuNumBits;
                bs->m_accuNumBits += 8;
                bs->m_pos++;
            }
        }

        static inline u8 nsrle_read_bits(nsrle_decoder_t *bs, u8 n)
        {
            u8 v = (u8)(bs->m_accuRegister & ((1UL << n) - 1));
            bs->m_accuRegister >>= n;
            bs->m_accuNumBits -= n;
            return v;
        }

        static inline u8 nsrle_read_symbol(nsrle_decoder_t *bs, const u8 *symbol_rb, u8 symbol_bits)
        {
            if (bs->m_symbol_run > 0)
            {
                bs->m_symbol_run--;
                return bs->m_symbol;
            }

            nsrle_check_accumulator(bs);

            const u8 symbol = nsrle_read_bits(bs, symbol_bits);
            const u8 rb     = symbol_rb[symbol];
            if (rb != 0)
            {
                bs->m_symbol     = symbol;
                bs->m_symbol_run = nsrle_read_bits(bs, rb);
            }
            return symbol;
        }

    }  // namespace nmui
}  // namespace ncore

#endif  // __ARDUINO_REMOTE_UI_NSRLE_STREAM_H__