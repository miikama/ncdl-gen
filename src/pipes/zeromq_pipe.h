
#pragma once

#include <memory>

#include <fmt/core.h>
#include <zmq.hpp>

#include "utils.h"
#include "vector_interface.h"

#include "zeromq_configuration.h"

namespace ncdlgen
{

/**
 * The metadata that is required to reconstruct ND arrays after receiving them
 */
struct ZeroMQVariableInfo
{
    std::string name{};
    std::vector<std::size_t> dimension_sizes{};

    std::string to_string();
    static ZeroMQVariableInfo from_string_view(const std::string_view);
};

template <typename ContainerType, typename ElementType, typename ContainerInterface>
zmq::message_t message_for_type(const ContainerType& data)
{
    if constexpr (std::is_fundamental_v<ContainerType>)
    {
        auto msg = zmq::message_t(&data, sizeof(ElementType));
        return msg;
    }
    else if constexpr (ContainerInterface::template is_supported_ndarray<ElementType, ContainerType>())
    {
        auto flat_data = ContainerInterface::template prepare<ElementType, ContainerType>(data);

        auto msg = zmq::message_t(flat_data.data.data(), flat_data.data.size() * sizeof(ElementType));
        return msg;
    }
    else
    {
        static_assert(always_false_v<ElementType>, "unsupported type");
    }
}

template <typename ContainerType, typename ElementType, typename ContainerInterface>
ContainerType data_from_message(const zmq::message_t& message, const ZeroMQVariableInfo& variable_info)
{
    if constexpr (std::is_fundamental_v<ContainerType>)
    {
        if (message.size() != sizeof(ElementType))
        {
            throw std::runtime_error(
                fmt::format("Incorrect size of input scalar message, expected size {}, received size {}",
                            sizeof(ElementType), message.size()));
        }
        return *message.data<ElementType>();
    }
    else if constexpr (ContainerInterface::template is_supported_ndarray<ElementType, ContainerType>())
    {
        auto flat_data =
            ContainerInterface::template prepare<ElementType, ContainerType>(variable_info.dimension_sizes);

        if (message.size() != sizeof(ElementType) * flat_data.data.size())
        {
            throw std::runtime_error(
                fmt::format("Incorrect size of input vector message, expected size {}, received size {}",
                            sizeof(ElementType) * flat_data.data.size(), message.size()));
        }
        auto data_ptr = message.data<ElementType>();
        flat_data.data.assign(data_ptr, data_ptr + message.size());

        // Format data from buffer to final container
        ContainerType data{};
        ContainerInterface::template finalise<ElementType, ContainerType>(data, flat_data);
        return data;
    }
    else
    {
        static_assert(always_false_v<ElementType>, "unsupported type");
    }
}

/**
 * Write and read data from zeromq.
 *
 * https://zeromq.org/get-started/?language=cpp#
 * https://github.com/zeromq/cppzmq
 */
class ZeroMQPipe
{
  public:
    ZeroMQPipe();
    ZeroMQPipe(const ZeroMQConfiguration& config);

    virtual ~ZeroMQPipe() = default;

    /**
     * Main inteface for writing data to socket
     *
     * The main idea is to for serialization of the data is to have a unique
     * id for each field we write. This allows structuring the data in the receiving end.
     *
     * Send multipart (twopart) message, where the first is the id and the second is
     * the data, https://brettviren.github.io/cppzmq-tour/index.html#orgefd4893
     *
     * Use the cdl full variable path as the id.
     */
    template <typename ContainerType, typename ElementType, typename ContainerInterface>
    void write(const std::string_view full_path, const ContainerType& data)
    {
        validate_name(full_path);

        // Get a socket for writing
        auto& socket = get_outbound_socket();

        auto data_size =
            VectorOperations::template container_dimension_sizes<ElementType, ContainerType>(data);
        ZeroMQVariableInfo variable_info{std::string(full_path), data_size};
        auto info_string = variable_info.to_string();

        auto id_message = zmq::message_t(info_string.data(), info_string.size());
        auto data_message = message_for_type<ContainerType, ElementType, ContainerInterface>(data);

        if (!socket.send(id_message, zmq::send_flags::sndmore))
        {
            throw std::runtime_error(
                fmt::format("Error sending a message with id {} with zeromq.", full_path));
        }
        if (!socket.send(data_message, zmq::send_flags::none))
        {
            throw std::runtime_error(
                fmt::format("Error sending a data message with id {} with zeromq.", full_path));
        }
    }

    /**
     * Main inteface for reading data from netcdf
     */
    template <typename ContainerType, typename ElementType, typename ContainerInterface>
    ContainerType read(const std::string_view full_path)
    {
        validate_name(full_path);

        // get socket for reading
        auto& socket = get_incoming_socket();

        zmq::message_t id_message;
        zmq::message_t data_message;
        auto res = socket.recv(id_message, zmq::recv_flags::none);
        if (!id_message.more())
        {
            throw std::runtime_error(
                fmt::format("Error receiving a message with id {} with zeromq.", full_path));
        }
        auto data_res = socket.recv(data_message, zmq::recv_flags::none);

        auto variable_info = ZeroMQVariableInfo::from_string_view(id_message.to_string_view());
        if (variable_info.name != full_path)
        {
            throw std::runtime_error(
                fmt::format("Received the id message with wrong id, expected '{}', received '{}", full_path,
                            variable_info.name));
        }

        auto data =
            data_from_message<ContainerType, ElementType, ContainerInterface>(data_message, variable_info);
        return data;
    }

    void validate_name(std::string_view name) const;

    /**
     * Create the sockets upon first usage
     */
    zmq::context_t& get_context();
    zmq::socket_t& get_incoming_socket();
    zmq::socket_t& get_outbound_socket();

  private:
    std::unique_ptr<zmq::context_t> m_context;
    std::unique_ptr<zmq::socket_t> m_incoming_socket;
    std::unique_ptr<zmq::socket_t> m_outbound_socket;

    ZeroMQConfiguration m_config{};
};

} // namespace ncdlgen
