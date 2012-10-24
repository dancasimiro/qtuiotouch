/// UNCLASSIFIED
#ifndef DOSC_UDP_RECEIVER_HEAD
#define DOSC_UDP_RECEIVER_HEAD

#include "dosc/bytes.hpp"
#include "dosc/decoder.hpp"
#include <iostream>
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

namespace dosc {

template <class DatagramSocket>
class datagram_receiver : public boost::enable_shared_from_this<datagram_receiver<DatagramSocket> > {
public:
        typedef DatagramSocket datagram_socket;
        typedef typename datagram_socket::protocol_type protocol_type;
        typedef typename datagram_socket::endpoint_type endpoint_type;

        datagram_receiver(boost::asio::io_service& service)
                : boost::enable_shared_from_this<datagram_receiver>()
                , buffer_(init_buffer_size)
                , socket_(service)
                , decode_()
        {
        }

        datagram_receiver(boost::asio::io_service& service, const protocol_type& proto)
                : boost::enable_shared_from_this<datagram_receiver>()
                , buffer_(init_buffer_size)
                , socket_(service, proto)
                , decode_()
        {
        }

        datagram_receiver(boost::asio::io_service& service, const endpoint_type& endpoint)
                : boost::enable_shared_from_this<datagram_receiver>()
                , buffer_(init_buffer_size)
                , socket_(service, endpoint)
                , decode_()
        {
        }

        virtual ~datagram_receiver()
        {
        }

        void start()
        {
                schedule_read();
        }
        
        void stop()
        {
                socket_.close();
        }
private:
        static const size_t init_buffer_size = 4096u;

        bytes_buffer_type buffer_;
        datagram_socket socket_;
        decoder decode_;

        void schedule_read()
        {
                socket_.async_receive(boost::asio::buffer(buffer_), 
                                      boost::bind(&datagram_receiver::handle_packet,
                                                  this->shared_from_this(),
                                                  boost::asio::placeholders::error,
                                                  boost::asio::placeholders::bytes_transferred));
        }

        void handle_packet(const boost::system::error_code& error, std::size_t bytes)
        {
                if (!error) {
                        assert(bytes % 4 == 0);
                        // make a copy of the data
                        const bytes_ptr copy = boost::make_shared<bytes_buffer_type>(buffer_.begin(), buffer_.begin() + bytes);
                        schedule_read();
                        decode_(copy);
                } else {
                        std::cerr << "Could not receive packet: " << error.message() << std::endl;
                }
        }

        template <class PacketReceiver, class Handler>
        friend void add_method(PacketReceiver& recv, const std::string& address_pattern, Handler h);
};     // class datagram_receiver

typedef datagram_receiver<boost::asio::ip::udp::socket> udp_receiver;

template <class PacketReceiver, class Handler>
void add_method(PacketReceiver& recv, const std::string& address_pattern, Handler h)
{
        return add_method(recv.decode_, address_pattern, h);
}
}      // namespace dosc
#endif // DOSC_UDP_RECEIVER_HEAD
