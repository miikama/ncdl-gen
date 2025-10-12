

#pragma once

#include <string>

namespace ncdlgen
{

struct ZeroMQConfiguration
{
    // By default, pick an explicit socket on random
    // We pick an explicit socket for sending instead of
    //   tcp://127.0.0.1:*
    // To allow listening to right socket by default
    std::string outbound_socket{"tcp://127.0.0.1:42042"};
    std::string incoming_socket{"tcp://127.0.0.1:42042"};
};

} // namespace ncdlgen