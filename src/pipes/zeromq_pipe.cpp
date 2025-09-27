

#include <zmq.hpp>

#include "pipes/zeromq_pipe.h"

namespace ncdlgen
{

std::string ZeroMQVariableInfo::to_string()
{
    std::ostringstream oss;
    for (size_t i = 0; i < dimension_sizes.size(); i++)
    {
        if (i > 0)
        {
            oss << ",";
        }
        oss << dimension_sizes[i];
    }
    return fmt::format("{};{}", name, oss.str());
}
ZeroMQVariableInfo ZeroMQVariableInfo::from_string_view(const std::string_view field_message)
{
    ZeroMQVariableInfo variable_info{};

    auto split = split_string(field_message, ';');
    if (split.size() > 2 || split.size() == 0)
    {
        throw std::runtime_error(fmt::format("ZeroMQVariableInfo: cannot create variable info from message "
                                             "{}. Expected exactly 1 ';', found {}.",
                                             field_message, split.size()));
    }

    variable_info.name = split[0];
    // If no dimension info available, assume scalar and set dimensions to 0
    if (split.size() == 1)
    {
        // NOTE: push back 1 to dimension info?
        return variable_info;
    }

    auto numbers = split_string(split[1], ',');
    for (auto number : numbers)
    {
        variable_info.dimension_sizes.push_back(std::stoi(std::string(number)));
    }
    return variable_info;
}

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
