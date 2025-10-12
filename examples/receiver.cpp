
#include <ncdlgen/netcdf_pipe.h>
#include <ncdlgen/zeromq_configuration.h>
#include <ncdlgen/zeromq_pipe.h>

#include "example_data.h"

#include <filesystem>
#include <fmt/core.h>

/**
 * Example receiver
 *
 * Will write the Data in a new example_N.nc file
 * each time we receive data over the socket.
 *
 * Pairs together with sender.cpp
 *
 * Demo:
 *   Start the receiver
 *   Execute sender N times
 *   Writes example_[1,N].nc files
 */
int main()
{
    // Create ZeroMQPipe at local socket
    ncdlgen::ZeroMQConfiguration config{.incoming_socket = "tcp://127.0.0.1:42043"};
    ncdlgen::ZeroMQPipe pipe(config);

    size_t count{1};

    while (true)
    {
        generated::Data data{};
        read(pipe, data);

        fmt::print("Read data\n");

        auto output_name = fmt::format("example_{}.nc", count);
        std::filesystem::copy_file("example.nc", output_name,
                                   std::filesystem::copy_options::overwrite_existing);

        ncdlgen::NetCDFPipe output_pipe{output_name};

        output_pipe.open();
        generated::write(output_pipe, data);
        output_pipe.close();
        count++;
    }

    return 0;
}