
# CDL parsing

parse Netcdf CDL format

There is a nice BNF tester [online](https://bnfplayground.pauliankline.com)

The CDL grammar is also available [here](https://manpages.ubuntu.com/manpages/focal/man1/ncgen.1.html)


## Build

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

