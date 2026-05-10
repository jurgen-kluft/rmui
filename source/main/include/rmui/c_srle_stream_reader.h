#ifndef __ARDUINO_REMOTE_UI_SRLE_STREAM_READER_H__
#define __ARDUINO_REMOTE_UI_SRLE_STREAM_READER_H__
#include "rcore/c_target.h"
#ifdef USE_PRAGMA_ONCE
    #pragma once
#endif

#include "rmui/c_bitstream.h"

namespace ncore
{
    namespace nmui
    {
        struct srlen_streamreader_t
        {
            bitstream_reader_t* m_bitstream;    // bitstream reader for reading symbol and run length bits
            u8 const*           m_rb;           // run-bits per symbol (indexed by symbol value)
            u8                  m_symbol_bits;  // number of bits for the current symbol (e.g. 8 for P8 stream, 4 for P4 stream, etc.)
            u8                  m_run_symbol;   // current run symbol
            u16                 m_run_length;   // current run length
        };

        void init(srlen_streamreader_t* r, bitstream_reader_t* bitstream, u8 const* rb, u8 symbol_bits);

        // read one symbol from the srle stream
        static inline s16 read_symbol(srlen_streamreader_t* r)
        {
            // if we still have a run length, return the run symbol
            if (r->m_run_length > 0)
            {
                r->m_run_length--;
                return r->m_run_symbol;
            }

            const s16 symbol = bsr_read_bits(r->m_bitstream, r->m_symbol_bits);

            // check the run-bit for this symbol
            const u8 run_bits = r->m_rb[symbol];
            if (run_bits)
            {
                r->m_run_symbol   = symbol;
                r->m_run_length   = (u32)bsr_read_bits(r->m_bitstream, run_bits);
            }

            return symbol;
        }

        static inline void decode(u8 const* encoded_stream, u32 encoded_stream_size, u8 const* rb, u8 symbol_bits, u8* decoded_stream, u32 decoded_stream_size)
        {
            bitstream_reader_t bs;
            bsr_init(&bs, (u8*)encoded_stream, 0, encoded_stream_size);

            srlen_streamreader_t srle_reader;
            init(&srle_reader, &bs, rb, symbol_bits);

            for (u32 i = 0; i < decoded_stream_size; i++)
            {
                decoded_stream[i] = (u8)read_symbol(&srle_reader);
            }
        }

    }  // namespace nmui
}  // namespace ncore

#endif  // __ARDUINO_REMOTE_UI_SRLE_STREAM_READER_H__