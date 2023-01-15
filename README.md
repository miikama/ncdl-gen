
# CDL parsing

parse Netcdf CDL format

There is a nice BNF tester [online](https://bnfplayground.pauliankline.com)

The CDL grammar is also available [here](https://manpages.ubuntu.com/manpages/focal/man1/ncgen.1.html)


## Install dependencies with conan

```sh
mkdir build
cd build
conan install --build=missing  -s build_type=Release -s compiler.libcxx=libstdc++11 ..
cmake ..
make -j6
```

> NOTE: add permanent setting with `conan profile update settings.compiler.libcxx=libstdc++11 default`

To build and run the tests, enable them separately by setting `-DBUILD_TESTING=ON`:

```sh
mkdir build
cd build
cmake -DBUILD_TESTING=ON ..
make
make test
```

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


## Manual dependency build instructions [Deprecated for conan build]

Dependencies

```
fmt
```

`fmt` installation instructions https://fmt.dev/latest/usage.html#installing-the-library

Basic build

```sh
mkdir build
cd build
cmake ..
make
```

Tests use googletest, install with `apt install libgtest-dev`. Tested with gtest `10.x`. To build tests, set `-DBUILD_TESTING=ON`:

```sh
mkdir build
cd build
cmake -DBUILD_TESTING=ON ..
make
make test
```
