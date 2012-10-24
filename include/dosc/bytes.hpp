/// UNCLASSIFIED
///
/// \brief Byte utitlities
#ifndef DOSC_BYTES_HEAD
#define DOSC_BYTES_HEAD 1
#include <utility>
#include <vector>
#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>
namespace dosc {
typedef unsigned char byte;
typedef std::vector<byte> bytes_buffer_type;
typedef boost::shared_ptr<bytes_buffer_type> bytes_ptr;
typedef bytes_buffer_type::iterator buffer_iterator;
typedef bytes_buffer_type::const_iterator const_buffer_iterator;
typedef std::pair<buffer_iterator, buffer_iterator> buffer_range;
typedef std::pair<const_buffer_iterator, const_buffer_iterator> const_buffer_range;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;

namespace detail {
inline uint16_t do_swap_bytes(const uint16_t input)
{
        const uint16_t out =
                ((input & 0xff00) >> 8) |
                ((input & 0x00ff) << 8)
                ;      
        return out;
}

inline uint32_t do_swap_bytes(const uint32_t input)
{
        const uint32_t out =
                ((input & 0xff000000) >> 24) |
                ((input & 0x00ff0000) >> 8) |
                ((input & 0x0000ff00) << 8) |
                ((input & 0x000000ff) << 24)
                ;      
        return out;
}

inline uint64_t do_swap_bytes(const uint64_t input)
{
        const uint64_t out =
                ((input & 0xff00000000000000LL) >> 56) |
                ((input & 0x00ff000000000000LL) >> 40) |
                ((input & 0x0000ff0000000000LL) >> 24) |
                ((input & 0x000000ff00000000LL) >> 8) |
                ((input & 0x00000000ff000000LL) << 8) |
                ((input & 0x0000000000ff0000LL) << 24) |
                ((input & 0x000000000000ff00LL) << 40) |
                ((input & 0x00000000000000ffLL) << 56)
                ;      
        return out;
}
} // namespace detail

template <typename T>
T swap_network_bytes_to_native_order(const T input)
{
#ifndef __BIG_ENDIAN__
        return detail::do_swap_bytes(input);
#else
        return input;
#endif
}

template <typename T>
T swap_native_bytes_to_network_order(const T input)
{
#ifndef WORDS_BIGENDIAN
        return detail::do_swap_bytes(input);
#else
        return input;
#endif
}

}      // namespace dosc
#endif // DOSC_BYTES_HEAD
