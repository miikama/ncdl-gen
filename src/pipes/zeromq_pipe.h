
#pragma once

#include <fmt/core.h>
#include <zmq.hpp>

#include "utils.h"
#include "vector_interface.h"

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

{

    if constexpr (std::is_fundamental_v<T>)
    {

        auto msg = zmq::message_t(&data, sizeof(T));
        return msg;
    }
    else
    {
        static_assert(always_false_v<T>, "unsupported type");
    }
}

template <typename T> T data_from_message(const zmq::message_t& message)
{
    if constexpr (std::is_fundamental_v<T>)
    {
        if (message.size() != sizeof(T))
        {
            throw std::runtime_error(
                fmt::format("Incorrect size of input scalar message, expected size {}, received size {}",
                            sizeof(T), message.size()));
        }
        return *message.data<T>();
    }
    else
    {
        static_assert(always_false_v<T>, "unsupported type");
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


            if (!m_outbound_socket.send(id_message, zmq::send_flags::sndmore))
            {
                throw std::runtime_error(
                    fmt::format("Error sending a message with id {} with zeromq.", full_path));
            }
            if (!m_outbound_socket.send(data_message, zmq::send_flags::none))
            {
                throw std::runtime_error(
                    fmt::format("Error sending a data message with id {} with zeromq.", full_path));
            }
        }
        else
        {
            static_assert(always_false_v<ContainerType>, "Unsupported type for writing to ZeroMQ");
        }
    }

    /**
     * Main inteface for reading data from netcdf
     */
    template <typename ContainerType, typename ElementType, typename ContainerInterface>
    ContainerType read(const std::string_view full_path)
    {
        validate_name(full_path);

        zmq::message_t id_message;
        zmq::message_t data_message;
        auto res = m_inbound_socket.recv(id_message, zmq::recv_flags::none);
        if (!id_message.more())
        {
            throw std::runtime_error(
                fmt::format("Error receiving a message with id {} with zeromq.", full_path));
        }
        auto data_res = m_inbound_socket.recv(data_message, zmq::recv_flags::none);

        auto received_path = id_message.to_string_view();
        if (received_path != full_path)
        {
            throw std::runtime_error(
                fmt::format("Received the id message with wrong id, expected '{}', received '{}", full_path,
                            received_path));
        }

        auto data = data_from_message<ContainerType>(data_message);
        return data;
    }

    void validate_name(std::string_view name) const;

  private:
    zmq::context_t m_context;
    zmq::socket_t m_inbound_socket;
    zmq::socket_t m_outbound_socket;
};

} // namespace ncdlgen
