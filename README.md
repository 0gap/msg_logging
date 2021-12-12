# msg_logging

Testing flatbuffers with zmq and grpc.

The idea is to see which setup is better:
- Logging directly into file
- Sending to a zmq service(co-located or remote)
- Sending to a grpc service(co-located or remote)

**Compiler:** `ubuntu 20.04, g++ 9.3`
**Conan:** `1.42.1`
**CMake:** `3.16.3`
