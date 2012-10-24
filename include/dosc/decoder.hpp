/// UNCLASSIFIED
#ifndef DOSC_DECODER_HEAD
#define DOSC_DECODER_HEAD

#include "bytes.hpp"

#include <string>
#include <map>
#include <boost/function.hpp>
#include <boost/cstdint.hpp>

namespace dosc {
using boost::int32_t;
using boost::int64_t;

enum osc_type_tags {
        osc_int32_tag = 'i',     // 4 bytes, int32_t
        osc_float_tag = 'f',     // 4 bytes, float
        osc_string_tag = 's',    // variable
        osc_blob_tag = 'b',      // variable
        osc_int64_tag = 'h',     // 8 bytes, int64_t
        osc_time_tag = 't',      // 8 bytes
        osc_double_tag = 'd',    // 8 bytes, double
        osc_alt_string_tag = 'S',// variable
        osc_char_tag = 'c',      // 4 bytes, char
        osc_rgba_tag = 'r',      // 4 bytes, 32 bit RGBA color
        osc_midi_tag = 'm',      // 4 byte MIDI message
        osc_true_tag = 'T',      // True. No bytes are allocated in the argument data
        osc_false_tag = 'F',     // False. No bytes are allocated in the argument data
        osc_nil_tag = 'N',       // Nil. No bytes are allocated in the argument data
        osc_infinitum_tag = 'I', // Infinitum. No bytes are allocated in the argument data
        osc_array_beg_tag = '[', // Indicates the beginning of an array
        osc_array_end_tag = ']', // Indicates the end of an array
};

void chomp_string(const_buffer_range& range, const size_t max_nul);

int32_t decode_int32_tag(const_buffer_range& range);
int64_t decode_int64_tag(const_buffer_range& range);
float   decode_float_tag(const_buffer_range& range);
double  decode_double_tag(const_buffer_range& range);
std::string decode_string_tag(const_buffer_range& range);

class decoder {
public:
        // callback function accepts: address, type_string, osc_arguments
        typedef boost::function<void (std::string, std::string, const_buffer_range)> function_type;

        static const std::string all;

        void operator()(const bytes_ptr packet);
private:
        typedef std::map<std::string, function_type> map_type;

        map_type methods_; ///< address methods; invoked when new messages arrive

        const_buffer_iterator parse(const_buffer_range);
        const_buffer_iterator parse_message(const_buffer_range);
        const_buffer_iterator parse_bundle(const_buffer_range);

        template <class Handler>
        friend void add_method(decoder& dcd, const std::string& address_pattern, Handler h);
};     // class decoder

template <class Handler>
void add_method(decoder& dcd, const std::string& address_pattern, Handler h)
{
        dcd.methods_[address_pattern] = h;
}

}      // namespace dosc
#endif // DOSC_DECODER_HEAD
