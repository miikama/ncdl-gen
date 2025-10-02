
#include "example_data.h"

int main()
{

    ncdlgen::data data{{0}, {30}, {50}, {4.3}};

    ncdlgen::NetCDFPipe pipe{"example.nc"};

    pipe.open();
    ncdlgen::write(pipe, data);
    pipe.close();

    return 0;
}