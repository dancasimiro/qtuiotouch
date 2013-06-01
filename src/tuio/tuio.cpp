/// UNCLASSIFIED
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "tuio/tuio.hpp"
#include <algorithm>
#include <boost/bind.hpp>

namespace tuio {

position_type::position_type()
        : x(0.0f)
        , y(0.0f)
        , z(0.0f)
{
}

angle_type::angle_type()
        : a(0.0f)
        , b(0.0f)
        , c(0.0f)
{
}

dimension_type::dimension_type()
        : width(0.0f)
        , height(0.0f)
        , depth(0.0f)
{
}

velocity_type::velocity_type()
        : X(0.0f)
        , Y(0.0f)
        , Z(0.0f)
        , A(0.0f)
        , B(0.0f)
        , C(0.0f)
{
}

acceleration_type::acceleration_type()
        : motion(0.0f)
        , rotation(0.0f)
{
}

touch_point::touch_point()
        : type(unknown_touch)
        , session_id(0)
        , class_id(0)
        , position()
        , angle()
        , dims()
        , area(0.0f)
        , volume(0.0f)
        , velocity()
        , acceleration()
{
}

frame::frame()
        : frame_id(0)
        , source()
        , added()
        , updated()
        , removed()
{
}

frame_decoder::frame_decoder()
        : framecb_()
        , frame_()
{
}

void
frame_decoder::proc_fseq(dosc::const_buffer_range& arguments)
{
        const int frame_id = dosc::decode_int32_tag(arguments);
        // only callback valid frame ID sequences. don't callback when the frame ID jumps
        if (frame_id != frame_.frame_id) {
                if (frame_id == frame_.frame_id + 1) {
                        framecb_(frame_);
                        frame_.removed.clear();
                        copy(frame_.added.begin(), frame_.added.end(), back_inserter(frame_.updated));
                        frame_.added.clear();
                }
                frame_.frame_id = frame_id;
        }
}

static bool match_session_id(const touch_point& tp, const int session_id)
{
        return tp.session_id == session_id;
}

void
frame_decoder::proc_set(const std::string& address, const std::string& type_string, dosc::const_buffer_range& args)
{
        // the leading ",s" was already removed from type_string
        assert(type_string[0] == 'i');
        const int session_id = dosc::decode_int32_tag(args);
        std::vector<touch_point>::iterator tp_iter = find_if(frame_.updated.begin(), frame_.updated.end(),
                                                             boost::bind(&match_session_id, _1, session_id));
        if (tp_iter == frame_.updated.end()) {
                tp_iter = find_if(frame_.added.begin(), frame_.added.end(),
                                  boost::bind(&match_session_id, _1, session_id));
                if (tp_iter == frame_.added.end()) {
                        touch_point my_touch_point;
                        my_touch_point.session_id = session_id;
                        frame_.added.push_back(my_touch_point);
                        tp_iter = frame_.added.end() - 1;
                }
        }
        assert(tp_iter->session_id == session_id);

        if (address == "/tuio/2Dobj") {
                assert(type_string == "iiffffffff");
                tp_iter->type = token_touch;
                tp_iter->class_id = dosc::decode_int32_tag(args);
                tp_iter->position.x = dosc::decode_float_tag(args);
                tp_iter->position.y = dosc::decode_float_tag(args);
                tp_iter->angle.a = dosc::decode_float_tag(args);
                tp_iter->velocity.X = dosc::decode_float_tag(args);
                tp_iter->velocity.Y = dosc::decode_float_tag(args);
                tp_iter->velocity.A = dosc::decode_float_tag(args);
                tp_iter->acceleration.motion = dosc::decode_float_tag(args);
                tp_iter->acceleration.rotation = dosc::decode_float_tag(args);
        } else if (address == "/tuio/2Dcur") {
                assert(type_string == "ifffff");
                tp_iter->type = pointer_touch;
                tp_iter->position.x = dosc::decode_float_tag(args);
                tp_iter->position.y = dosc::decode_float_tag(args);
                tp_iter->velocity.X = dosc::decode_float_tag(args);
                tp_iter->velocity.Y = dosc::decode_float_tag(args);
                tp_iter->acceleration.motion = dosc::decode_float_tag(args);
        } else if (address == "/tuio/2Dblb") {
                assert(type_string == "ifffffffffff");
                tp_iter->type = pointer_touch;
                tp_iter->position.x = dosc::decode_float_tag(args);
                tp_iter->position.y = dosc::decode_float_tag(args);
                tp_iter->angle.a = dosc::decode_float_tag(args);
                tp_iter->dims.width = dosc::decode_float_tag(args);
                tp_iter->dims.height = dosc::decode_float_tag(args);
                tp_iter->area = dosc::decode_float_tag(args);
                tp_iter->velocity.X = dosc::decode_float_tag(args);
                tp_iter->velocity.Y = dosc::decode_float_tag(args);
                tp_iter->acceleration.motion = dosc::decode_float_tag(args);
                tp_iter->acceleration.rotation = dosc::decode_float_tag(args);
        } else {
                std::cout << "WARNING: Unknown TUIO Address!" << std::endl;
                assert(false);
        }
}

void
frame_decoder::proc_alive(const std::string& type_string, dosc::const_buffer_range& args)
{
        // decode all of the session IDs in the "alive" command
        std::vector<int> current_session_ids;
        // the leading ",s" was already removed from type_string
        for (std::string::const_iterator i = type_string.begin(); i != type_string.end(); ++i) {
                assert(*i == 'i');
                current_session_ids.push_back(dosc::decode_int32_tag(args));
        }

        // look for stuff that was removed. (this needs to be optimized; brute force here...)
        for (std::vector<touch_point>::iterator i = frame_.updated.begin(); i != frame_.updated.end();) {
                if (find(current_session_ids.begin(), current_session_ids.end(), i->session_id) == current_session_ids.end()) {
                        frame_.removed.push_back(*i);
                        i = frame_.updated.erase(i);
                } else {
                        ++i;
                }
        }
}

void
frame_decoder::operator()(const std::string& address, const std::string& type_string, dosc::const_buffer_range arguments)
{
        // get the command from the arguments
        assert(type_string[1] == 's');
        const char* const command = dosc::decode_string_tag(arguments);
        assert(command);
        if (!command) {
                std::cout << "Warning: Invalid TUIO stream!" << std::endl;
                return;
        }
        if (!strcmp(command, "set")) {
                proc_set(address, type_string.substr(2), arguments);
        } else if (!strcmp(command, "alive")) {
                proc_alive(type_string.substr(2), arguments);
        } else if (!strcmp(command, "fseq")) {
                assert(type_string == ",si");
                proc_fseq(arguments);
        } else if (!strcmp(command, "source")) {
                assert(type_string == ",ss");
                frame_.source = dosc::decode_string_tag(arguments);
        } else {
                std::cout << "Warning: Unknown TUIO command!" << std::endl;
        }
}

} // namespace tuio
