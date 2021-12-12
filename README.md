# msg_logging

Testing flatbuffers with zmq and grpc.

The idea is to see which setup is better:
- Logging directly into file
- Sending to a zmq service(co-located or remote)
- Sending to a grpc service(co-located or remote)


### Tested with
| Requirement                    | Version                   |
|--------------------------------|---------------------------|
| **Compiler**                   | `ubuntu 20.04, g++ 9.3`   |
| **[Conan](https://conan.io/)** | `1.42.1`                  |
| **CMake**                      | `3.16.3`                  |
| **GNU Make**                   | `4.2.1`                   |
| **Docker**                     | `20.10.11, build dea9396` |
| **docker-compose**             | `1.25.0`                  |

#### Code dependencies
See `conanfile.txt` for code dependencies downloaded.
Conan handles all code dependencies. You don't install anything except 
  compiler, Conan(`pip install conan`), Cmake and Make.

You'll need to create two [Conan profiles](https://docs.conan.io/en/latest/reference/profiles.html#examples):
Here are two, one for debug one for release

`cat ~/.conan/profiles/debug`
```
[settings]
os=Linux
os_build=Linux
arch=x86_64
arch_build=x86_64
compiler=gcc
compiler.version=9
compiler.libcxx=libstdc++11
build_type=Debug
[options]
[build_requires]
[env]
```

` cat ~/.conan/profiles/release`
```
[settings]
os=Linux
os_build=Linux
arch=x86_64
arch_build=x86_64
compiler=gcc
compiler.version=9
compiler.libcxx=libstdc++11
build_type=Release
[options]
[build_requires]
[env]
```

CLion's Conan plugin gives the ability to map conan profiles to CMake build types
(Release to release etc.)

See `Makefile` if you are having trouble with dependencies and flatbuffer
files inclusion. If you have Conan installed and its profiles at place, running 
`make` should build everything.
