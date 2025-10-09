

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
    // If the incoming socket is not yet created at the time
    // of sending the data through output, data is not received
    //
    // Create the context and sockets at object creation
    // to enable seamless usage.
    //
    // This behaviour should likely be revisited
    get_outbound_socket();
    get_incoming_socket();
}

ZeroMQPipe::ZeroMQPipe(const ZeroMQConfiguration& config) : m_config(config)
{
    // If the incoming socket is not yet created at the time
    // of sending the data through output, data is not received
    //
    // Create the context and sockets at object creation
    // to enable seamless usage.
    //
    // This behaviour should likely be revisited
    get_outbound_socket();
    get_incoming_socket();
}

zmq::context_t& ZeroMQPipe::get_context()
{
    if (!m_context)
    {
        m_context = std::make_unique<zmq::context_t>();
    }
    return *m_context;
}
zmq::socket_t& ZeroMQPipe::get_outbound_socket()
{
    auto& context = get_context();

    if (!m_outbound_socket)
    {
        m_outbound_socket = std::make_unique<zmq::socket_t>(context, zmq::socket_type::push);
        m_outbound_socket->bind(m_config.outbound_socket);
    }
    return *m_outbound_socket;
}

zmq::socket_t& ZeroMQPipe::get_incoming_socket()
{
    auto& context = get_context();

    if (!m_incoming_socket)
    {
        m_incoming_socket = std::make_unique<zmq::socket_t>(context, zmq::socket_type::pull);
        m_incoming_socket->connect(m_config.incoming_socket);
    }
    return *m_incoming_socket;
}

void ZeroMQPipe::validate_name(std::string_view name) const
{
    if (name.find(';') != std::string_view::npos)
    {
        throw std::runtime_error(fmt::format("ZeroMQPipe: invalid field name, cannot contain ';'"));
    }
}

} // namespace ncdlgen
