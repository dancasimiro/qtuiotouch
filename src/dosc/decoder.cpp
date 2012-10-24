// UNCLASSIFIED
#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "dosc/decoder.hpp"
#include <stdexcept>
#include <algorithm>
#include <boost/range/algorithm/find.hpp>

namespace dosc {
const std::string decoder::all = "";

struct osc_time {
        uint32_t seconds; // since midnight 1 Jan 1900
        uint32_t frac;    // fractional seconds
};

void
chomp_string(const_buffer_range& range, const size_t max_nul)
{
        const_buffer_iterator end = range.first;
        advance(end, max_nul);
        end = min(end, range.second);
        for (; range.first != end && *range.first == '\0'; advance(range.first, 1)) ;
}

// calculate the size of the arguments and return the iterator that indicates the end.
static const_buffer_iterator calculate_size(const std::string& type_string, const_buffer_range range)
{
        const const_buffer_iterator start = range.first;
        for (std::string::const_iterator i = type_string.begin(); i != type_string.end(); ++i) {
                if (range.first >= range.second) {
                        return range.second;
                }
                switch (*i) {
                case osc_int32_tag:
                case osc_float_tag:
                case osc_char_tag:
                case osc_rgba_tag:
                case osc_midi_tag:
                        advance(range.first, 4);
                        break;
                case osc_int64_tag:
                case osc_time_tag:
                case osc_double_tag:
                        advance(range.first, 8);
                        break;
                case osc_string_tag:
                case osc_alt_string_tag: {
                        const const_buffer_iterator start = range.first;
                        while (range.first < range.second && *range.first != '\0') {
                                advance(range.first, 1);
                        }
                        chomp_string(range, 4 - distance(start, range.first) % 4);
                }
                        break;
                case osc_blob_tag:
                {
                        int32_t size = 0;
                        std::copy(range.first, range.first + 4, reinterpret_cast<char*>(&size));
                        advance(range.first, 4 + size);
                        if (int extra = size % 4) {
                                advance(range.first, 4 - extra);
                        }
                }
                        break;
                case osc_array_beg_tag:
                case osc_array_end_tag:
                        assert(false); // not implemented yet
                        break;
                case osc_true_tag:
                case osc_false_tag:
                case osc_nil_tag:
                case osc_infinitum_tag:
                default:
                        break;
                }
        }
        return range.first;
}

int32_t
decode_int32_tag(const_buffer_range& range)
{
        if (boost::size(range) < 4) {
                throw std::runtime_error("range is too small");
        }
        int32_t tag;
        std::reverse_copy(range.first, range.first + 4, reinterpret_cast<char*>(&tag));
        advance(range.first, 4);
        return tag;
}

int64_t
decode_int64_tag(const_buffer_range& range)
{
        if (boost::size(range) < 8) {
                throw std::runtime_error("range is too small");
        }
        int64_t tag;
        std::reverse_copy(range.first, range.first + 8, reinterpret_cast<char*>(&tag));
        advance(range.first, 8);
        return tag;
}

float
decode_float_tag(const_buffer_range& range)
{
        if (boost::size(range) < 4) {
                throw std::runtime_error("range is too small");
        }
        float tag;
        std::reverse_copy(range.first, range.first + 4, reinterpret_cast<char*>(&tag));
        advance(range.first, 4);
        return tag;
}

double
decode_double_tag(const_buffer_range& range)
{
        if (boost::size(range) < 8) {
                throw std::runtime_error("range is too small");
        }
        double tag;
        std::reverse_copy(range.first, range.first + 8, reinterpret_cast<char*>(&tag));
        advance(range.first, 8);
        return tag;
}

std::string
decode_string_tag(const_buffer_range& range)
{
        const_buffer_iterator null_pos = boost::find(range, '\0');
        if (null_pos == range.second) {
                throw std::runtime_error("Invalid string!");
        }
        const std::string tag(range.first, null_pos);
        range.first = null_pos;
        chomp_string(range, 4 - (tag.length() % 4));
        return tag;
}

void
decoder::operator()(const bytes_ptr packet)
{
        parse(make_pair(packet->begin(), packet->end()));
}

const_buffer_iterator
decoder::parse(const_buffer_range range)
{
        while (range.first < range.second) {
                switch (*range.first) {
                case '#': // bundle
                        //std::cout << "bundle" << std::endl;
                        range.first = parse_bundle(range);
                        //std::cout << "bundle done" << std::endl;
                        break;
                case '/': // message
                        //std::cout << "message" << std::endl;
                        range.first = parse_message(range);
                        //std::cout << "message done" << std::endl;
                        break;
                default:
                        //std::cout << "Warning: unrecognized character @ start of element: 0x" << static_cast<unsigned int>(*range.first) << std::endl;
                        advance(range.first, 1);
                }
        }
        //std::cout << "packet done" << std::endl;
        return range.first;
}

const_buffer_iterator
decoder::parse_message(const_buffer_range range)
{
        std::string address_pattern;
        for (; range.first != range.second && *range.first; advance(range.first, 1)) {
                address_pattern.push_back(*range.first);
        }
        chomp_string(range, 4 - address_pattern.size() % 4);

        // type string is optional
        std::string type_string;
        if (range.first != range.second && *range.first == ',') {
                for (; range.first != range.second && *range.first != '\0'; advance(range.first, 1)) {
                        type_string.push_back(*range.first);
                }
                chomp_string(range, 4 - type_string.size() % 4);
        }
        const_buffer_iterator arg_end = calculate_size(type_string, range);
        const_buffer_range arguments(range.first, arg_end);
        methods_[all](address_pattern, type_string, arguments);

        return arg_end;
}

static const_buffer_iterator parse_time(const_buffer_range range, osc_time& time)
{
        advance(range.first, 8);
        return range.first;
}

const_buffer_iterator
decoder::parse_bundle(const_buffer_range range)
{
        if (std::string(range.first, range.first + 7) != "#bundle") {
                std::cout << "Warning: bad bundle identifier" << std::endl;
                advance(range.first, 1);
                return range.first;
        }
        advance(range.first, 8); // length of "#bundle" + '\0' terminator

        osc_time time;
        range.first = parse_time(range, time);

        // parse the bundle elements
        while (range.first < range.second) {
                int32_t element_size = 0;
                const const_buffer_iterator size_start = range.first;
                advance(range.first, 4);
                std::reverse_copy(size_start, range.first, reinterpret_cast<char*>(&element_size));
                parse(range);
                assert(element_size % 4 == 0);
                while (element_size % 4) {
                        element_size += 1;
                }
                advance(range.first, element_size);
        }
        return range.first;
}

} // namespace dosc
