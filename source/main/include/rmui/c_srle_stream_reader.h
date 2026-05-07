#ifndef __ARDUINO_REMOTE_UI_SRLE_STREAM_READER_H__
#define __ARDUINO_REMOTE_UI_SRLE_STREAM_READER_H__
#include "rcore/c_target.h"
#ifdef USE_PRAGMA_ONCE
    #pragma once
#endif

namespace ncore
{
    namespace nmui
    {
        struct srle_stream_reader_t
        {
            u8 const* m_rb;
            u8 const* m_bitstream;    // pointer to the bitstream data
            u32       m_max_units;    // maximum number of units that can be read from the bitstream (used for bounds checking)
            u32       m_count;        // number of units read so far (used for bounds checking)
            u32       m_pos;          // current position in the bitstream (in bytes)
            u32       m_accu_reg;     // bit accumulator for reading bits
            u16       m_accu_bits;    // number of valid bits in the accumulator
            u8        m_symbol_bits;  // number of bits for the current symbol (e.g. 8 for P8 stream, 4 for P4 stream, etc.)
            u8        m_run_symbol;   // current run symbol
            u32       m_run_length;   // current run length
        };

        void init(srle_stream_reader_t* r, u8 const* rb, u8 const* bitstream, u32 max_units, u8 symbol_bits);

        static inline s16 read_bits(srle_stream_reader_t* r, u8 bits)
        {
            // ensure we have enough bits in the accumulator
            while (r->m_accu_bits < bits)
            {
                if (r->m_count >= r->m_max_units)
                {
                    // we've read the maximum number of units, return an error value
                    return -1;
                }

                // read the next unit from the bitstream and add it to the accumulator
                r->m_accu_reg |= (u32(r->m_bitstream[r->m_pos]) << r->m_accu_bits);
                r->m_accu_bits += 8;
                r->m_pos++;
                r->m_count++;
            }

            // extract the requested number of bits from the accumulator
            const s16 result = r->m_accu_reg & ((1 << bits) - 1);
            r->m_accu_reg >>= bits;
            r->m_accu_bits -= bits;

            return result;
        }

        // read one symbol from the srle stream
        static inline s16 read_symbol(srle_stream_reader_t* r)
        {
            if (r->m_count >= r->m_max_units)
                return -1;  // end of stream

            // if we still have a run length, return the run symbol
            if (r->m_run_length > 0)
            {
                r->m_run_length--;
                r->m_count++;
                return r->m_run_symbol;
            }

            if (r->m_accu_bits < r->m_symbol_bits)
                return -1;  // not enough bits to read a symbol

            const s16 symbol = read_bits(r, r->m_symbol_bits);

            // check the run-bit for this symbol
            const u8 run_bits = r->m_rb[symbol];
            if (run_bits)
            {
                r->m_run_symbol   = symbol;
                const s16 run_len = read_bits(r, run_bits);
                r->m_run_length   = run_len;
            }

            r->m_count++;
            return symbol;
        }

    }  // namespace nmui
}  // namespace ncore

#endif  // __ARDUINO_REMOTE_UI_SRLE_STREAM_READER_H__