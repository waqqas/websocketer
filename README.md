# Websocketer

A header-only websocket client library build on Boost::Beast

Building library
---

- `mkdir build`
- `cd build`
- `cmake -DCMAKE_INSTALL_PREFIX:PATH=`pwd`/usr ..`
- `cmake --build . --config Release --target install -- -j $(nproc)`

Building examples
---

After building library
- `cmake -DWEBSOCKETER_BUILD_EXAMPLES:BOOL=ON ..`
- `cmake --build . --config Release -- -j $(nproc)`

Running example
---

- `./examples/example1`

Usage
---

Steps

```

```