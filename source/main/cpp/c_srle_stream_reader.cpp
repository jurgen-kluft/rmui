#include "rmui/c_srle_stream_reader.h"

namespace ncore
{
    namespace nmui
    {
        void init(srle_stream_reader_t* r, u8 const* rb, u8 const* bitstream, u32 max_units, u8 symbol_bits)
        {
            r->m_rb          = rb;
            r->m_bitstream   = bitstream;
            r->m_max_units   = max_units;
            r->m_count       = 0;
            r->m_pos         = 0;
            r->m_accu_reg    = 0;
            r->m_accu_bits   = 0;
            r->m_symbol_bits = symbol_bits;
            r->m_run_symbol  = 0;
            r->m_run_length  = 0;
        }

    }  // namespace nmui
}  // namespace ncore
