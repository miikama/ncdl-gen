
#include <ncdlgen/zeromq_configuration.h>
#include <ncdlgen/zeromq_pipe.h>

#include "example_data.h"

/**
 * Example sender
 *
 * Send Data over local socket
 *
 * Pairs together with receiver.cpp
 *
 * Demo:
 *   Start the receiver
 *   Execute sender N times
 *   Writes example_[1,N].nc files
 */
int main()
{
    // prepare data (generated during compilation)
    generated::Data data{{0}, {30}, {50}, {4.3}};

    // Create ZeroMQPipe at local socket
    ncdlgen::ZeroMQConfiguration config{.outbound_socket = "tcp://127.0.0.1:42043"};
    ncdlgen::ZeroMQPipe pipe(config);

    // send data over. Hangs until data is received.
    generated::write(pipe, data);

    return 0;
}