#ifndef _SqlArray2_converts_h_
#define _SqlArray2_converts_h_
#include "lookups.h" // Added line

using namespace Upp;

LookupHdr(MAKER,ID,MAKER_NAME);

template<typename E>
constexpr auto toUnderlying(E e) noexcept {
    return static_cast<std::underlying_type_t<E>>(e);
}
#endif
