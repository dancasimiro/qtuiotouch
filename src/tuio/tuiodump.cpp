/// UNCLASSIFIED
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif


#include "dosc/decoder.hpp"
#include "dosc/asio/udp_receiver.hpp"
#include "tuio/tuio.hpp"
#include <boost/ref.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>

static void print_frame(const tuio::frame& frm)
{
        std::cout << "TUIO " << frm.source << " Frame " << frm.frame_id << ": \n";

        std::cout << "\tAdded: ";
        typedef std::vector<tuio::touch_point>::const_iterator iter;
        for (iter i = frm.added.begin(); i != frm.added.end(); ++i) {
                std::cout << i->session_id << "(" << i->position.x << "," << i->position.y << "), ";
        }
        std::cout << "\n";

        std::cout << "\tUpdated: ";
        for (iter i = frm.updated.begin(); i != frm.updated.end(); ++i) {
                std::cout << i->session_id << "(" << i->position.x << "," << i->position.y << "), ";
        }
        std::cout << "\n";

        std::cout << "\tRemoved: ";
        for (iter i = frm.removed.begin(); i != frm.removed.end(); ++i) {
                std::cout << i->session_id << "(" << i->position.x << "," << i->position.y << "), ";
        }
        std::cout << "\n";

        std::cout << "DONE" << std::endl;
}

int
main(int /*argc*/, char** /*argv*/)
{
        using namespace boost::asio;
        const short port = 3333; // default port for TUIO/OSC
        try {
                io_service service;

                boost::shared_ptr<dosc::udp_receiver> recv = boost::make_shared<dosc::udp_receiver>(boost::ref(service), ip::udp::endpoint(ip::udp::v4(), port));
                tuio::frame_decoder decoder;
                decoder.set_frame_callback(&print_frame);
                dosc::add_method(*recv, dosc::decoder::all, boost::ref(decoder));
                recv->start();

                service.run();
        } catch (const std::exception& ex) {
                std::cerr << "Fatal exception: " << ex.what() << std::endl;
                return 1;
        }
        return 0;
}

