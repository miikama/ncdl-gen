

#include <zmq.hpp>

#include "pipes/zeromq_pipe.h"

namespace ncdlgen
{

ZeroMQPipe::ZeroMQPipe()
{
    m_context = zmq::context_t();
    m_outbound_socket = zmq::socket_t(m_context, zmq::socket_type::push);
    m_inbound_socket = zmq::socket_t(m_context, zmq::socket_type::pull);

    // TODO: both the inbound and outbound sockets need to be arguments
    // This is enough to test the data serialisation, but not more.
    m_outbound_socket.bind("tcp://127.0.0.1:*");
    const std::string last_endpoint = m_outbound_socket.get(zmq::sockopt::last_endpoint);
    m_inbound_socket.connect(last_endpoint);
}

} // namespace ncdlgen
