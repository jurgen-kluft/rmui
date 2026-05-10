#include "rmui/c_srle_stream_reader.h"

namespace ncore
{
    namespace nmui
    {
        void init(srlen_streamreader_t* r, bitstream_reader_t* bitstream, u8 const* rb, u8 symbol_bits)
        {
            r->m_rb          = rb;
            r->m_bitstream   = bitstream;
            r->m_symbol_bits = symbol_bits;
            r->m_run_symbol  = 0;
            r->m_run_length  = 0;
        }

    }  // namespace nmui
}  // namespace ncore
