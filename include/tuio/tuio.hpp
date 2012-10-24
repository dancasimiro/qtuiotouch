/// UNCLASSIFIED
#ifndef TUIO_HEAD
#define TUIO_HEAD

#include "dosc/decoder.hpp"
#include <string>
#include <vector>
#include <boost/function.hpp>

namespace tuio {

enum touch_type {
        unknown_touch = 0, // unclassified
        token_touch,       // object such as a stylus
        pointer_touch,     // finger
};

struct position_type {
        float x, y, z;

        position_type();
};

struct angle_type {
        float a, b, c;

        angle_type();
};

struct dimension_type {
        float width, height, depth;

        dimension_type();
};

struct velocity_type {
        float X, Y, Z;
        float A, B, C; // rotation;

        velocity_type();
};

struct acceleration_type {
        float motion, rotation;

        acceleration_type();
};

struct touch_point {
        touch_type type;

        int session_id;
        int class_id;

        position_type position;
        angle_type angle;
        dimension_type dims;
        float area;
        float volume;
        velocity_type velocity;
        acceleration_type acceleration;

        touch_point();
};

// collect an entire frame?

struct frame {
        int frame_id;
        std::string source;
        std::vector<touch_point> added;   // touch points that are new to this frame
        std::vector<touch_point> updated; // currently in contact with sensor
        std::vector<touch_point> removed; // touch points that were lifted in this frame

        frame();
};

// provide callback that is invoked whenever a full frame is ready?
class frame_decoder {
public:
        frame_decoder();

        // invoked whenever a new frame is decoded.
        typedef boost::function<void (const frame&)> update_callback;

        template <typename Handler>
        void set_frame_callback(Handler h) { framecb_ = h; }

        void operator()(const std::string& address, const std::string& type_string, dosc::const_buffer_range arguments);
private:
        update_callback framecb_;
        frame frame_;

        void proc_fseq(dosc::const_buffer_range& args);
        void proc_set(const std::string& address, const std::string& type_string, dosc::const_buffer_range& args);
        void proc_alive(const std::string& type_string, dosc::const_buffer_range& args);
};

}      // namespace tuio
#endif // TUIO_HEAD
