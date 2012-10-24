/// UNCLASSIFIED
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "dosc/decoder.hpp"
#include "dosc/asio/udp_receiver.hpp"
#include <iostream>
#include <boost/range/size.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/ref.hpp>
#include <boost/asio.hpp>

static void handle_osc_message(const std::string& address, const std::string& type_string, dosc::const_buffer_range osc_arguments)
{
        std::cout << address << ": ";
        if (!type_string.empty()) {
                std::cout << "(" << type_string << ") ";
        }
        std::cout << "sent " << boost::size(osc_arguments) << " bytes:: ";
        for (std::string::const_iterator i = type_string.begin(); i != type_string.end(); ++i) {
                switch (*i) {
                case dosc::osc_int32_tag:
                        std::cout << dosc::decode_int32_tag(osc_arguments) << ", ";
                        break;
                case dosc::osc_float_tag:
                        std::cout << dosc::decode_float_tag(osc_arguments) << ", ";
                        break;
                case dosc::osc_string_tag:
                        std::cout << dosc::decode_string_tag(osc_arguments) << ", ";
                        break;
                case ',': // signifies start of type string
                        break;
                default:
                        std::cout << "?(" << *i << "), ";
                        break;
                }
        }
        std::cout << std::endl;
}

int
main(int /*argc*/, char** /*argv*/)
{
        using namespace boost::asio;
        const short port = 3333; // default port for TUIO/OSC
        try {
                io_service service;

                boost::shared_ptr<dosc::udp_receiver> recv = boost::make_shared<dosc::udp_receiver>(boost::ref(service), ip::udp::endpoint(ip::udp::v4(), port));
                dosc::add_method(*recv, dosc::decoder::all, &handle_osc_message); // handle all osc messages
                recv->start();

                service.run();
        } catch (const std::exception& ex) {
                std::cerr << "Fatal exception: " << ex.what() << std::endl;
                return 1;
        }
        return 0;
}
