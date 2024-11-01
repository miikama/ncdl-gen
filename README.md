# ncdlgen

ncdlgen is a C++ library that provides a [NetCDF](https://github.com/Unidata/netcdf-c) cdl-_parser_ and a _code generator_ for C++ code that reads/writes NetCDF files.

NetCDF is a platform independent data file format for structured data. It is commonly used in Earth observation missions as data storage format. NetCDF cdl-files describe structured data and are used together with the NetCDF library tools. The Netcdf CDL grammar is available [here](https://manpages.ubuntu.com/manpages/focal/man1/ncgen.1.html).

In the future ncdlgen could be used as a code generation tool for other structured data formats as well.

## Installation

Actually building `ncdlgen`

```sh
mkdir build
cd build
conan install --build=missing -of . ..
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=~/ncdlgen -DCMAKE_PREFIX_PATH=$(pwd) ..
make -j6 && make install
```

Note the usage of conan toolchain file which creates all the required packageConfig.cmake files in the build directory for finding the dependencies. It is also possible to manually download all the primary and transitive dependencies without Conan. Currently primary dependencies are

- fmt
- netCDF
- GTest

> NOTE: add permanent setting with `conan profile update settings.compiler.libcxx=libstdc++11 default`

To build and run the tests, enable them separately by setting `cmake -DBUILD_TESTING=ON .. && make && make test`.

## Parser

Take an example cdl-file

```
netcdf simple {

    group: foo {

        dimensions:
            dim = 5 ;

        variables:
            int bar ;
            float baz ;
            ushort bee(dim) ;
            int foobar(dim, dim) ;
    }
}
```

Result of parsing the file contents:

```sh
${installation_directory}/parser data/simple.cdl
Group simple
  Group foo
      Dimensions
          dim = 5
      Variables
          int bar
          float baz
          ushort bee (dim)
          int foobar (dim, dim)
```

## Code generator

Take the same example `data/simple.cdl` file but use it as an input for the code-generator:

```sh
${installation_directory}/generator data/simple.cdl --header > generated_simple.h
```

results in the following generated code

```c++
#pragma once

#include "stdint.h"

#include <vector>

#include "netcdf_interface.h"
#include "vector_interface.h"

namespace ncdlgen {

struct simple
{
  struct foo
  {
      int bar;
      float baz;
      std::vector<ushort> bee;
      std::vector<std::vector<int>> foobar;
  };

  foo foo_g{};
};

void read(NetCDFInterface& interface, simple&);

void read(NetCDFInterface& interface, simple::foo&);

void write(NetCDFInterface& interface, const simple&);

void write(NetCDFInterface& interface, const simple::foo&);

};
```

Which can be used to read/write the contents of the entire file or its subgroups

```c++
#include "generated_simple.h"

ncdlgen::simple root;
ncdlgen::NetCDFInterface interface{"generated.nc"};
interface.open();

// Write contents of 'simple' struct to a netcdf file
ncdlgen::write(interface, root);

// Read the contents of a netcdf file into 'simple' struct
read(interface, root);
interface.close();
```

## ncdlgen as dependency

After installing ncdlgen you can use the library in your projects `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.16)
project("ncdlgen-examples")

find_package(ncdlgen REQUIRED)

add_executable(custom_parser custom_parser.cpp)
target_link_libraries(custom_parser PRIVATE ncdlgen::ncdlgen)
```

This requires all the `ncdlgen` dependencies to be installed on the system as well.

If you used conan to dowload `ncdlgen` dependencies you can use the same libraries here. Copy the names of the dependencies from the `ncdlgen` repository `conanfile.txt` to a `conanfile.txt` in your project. Use that to set up the build chain

> This is just a hack until `ncdlgen` is available as conan package.

```sh
cd <your-project-root>
mkdir build
cd build
conan install ..
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=$(pwd) -DCMAKE_INSTALL_PREFIX=~/ncdlgen -DCMAKE_TOOLCHAIN_PATH=conan_toolchain.cmake ..
```

If you downloaded depencies manually, you can skip the `conan install`, leave out `-DCMAKE_TOOLCHAIN_PATH=conan_toolchain.cmake` and adding build directory to `CMAKE_PREFIX_PATH` parts.

## Build using Docker

There is a `Dockerfile` that setups a build environment for the current user. Build the docker file with

```sh
docker build \
    --build-arg USER=$(whoami) \
    --build-arg USER_ID=$(id -u $(whoami)) \
    --build-arg GROUP_ID=$(id -g $(whoami)) \
    --tag=ncdgen \
    .
```

After building, start the container with

```sh
docker run --rm -it -v $(pwd):/home/$(whoami) ncdgen bash
```

This mounts the repository at the home directory of the container user.

## Changelist

Main features for each release

0.1.0

- Initial relase with NetCDF cdl parser
- Code generator for generating code for interface reading/writing

0.2.0

- Support Conan 2
- Support multidimensional containers in interfaces
- Support multidimensional containers in code generation
- Update gtest version
- Improve code generation configurability
- Support global attributes outside of variables: section
- Resolve untyped attribute types by finding corresponding variable
- Make NetCDF and optional dependency

## Building VSCode extension

Official guides

[Getting started](https://code.visualstudio.com/api/get-started/your-first-extension) with extensions

[VSCode language extensions](https://code.visualstudio.com/api/language-extensions/syntax-highlight-guide)

The language extensions add a `language` [Contribution Point](https://code.visualstudio.com/api/references/contribution-points)

Text mate grammar guides

[Writing grammar](https://macromates.com/manual/en/language_grammars)

[Notes about textmate language grammar](https://www.apeth.com/nonblog/stories/textmatebundle.html)

Some inspiration is derived from the first example I found, which is the Jakt language in SerenityOS

[Jakt language syntax](https://github.com/SerenityOS/jakt/blob/main/editors/vscode/syntaxes/jakt.tmLanguage.json)
