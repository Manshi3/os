#include "util/io.h"

namespace vgatext{

    static inline void writechar(int loc, uint8_t c, uint8_t bg, uint8_t fg, addr_t base)
    {
        mmio::write16(base,2*loc,(bg<<12) | (fg<<8) | (c));
    }

}//namespace vgatext
