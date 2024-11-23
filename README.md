## ez-cpp
C++ tools and experimentation playground.

### Libraries
#### ez.core
[Core C++ tools](src/libs/core/README.md)

#### ez.rpc
[Asynchrounous RPC framework library](src/libs/rpc/README.md)

#### ez.flow
Interpreter and virtual machine for the experimental flow async programming language.
### Build instructions
#### Requirements
* A C++23 compliant compiler
* CMake 3.19 or more recent
* Conan 2.x 

#### Building with CMake

```
mkdir build/debug
cd build/debug
conan install -of . --build missing -s build_type=Debug ../..
cmake ../.. -DCMAKE_TOOLCHAIN_FILE="build/Debug/generators/conan_toolchain.cmake" -DCMAKE_BUILD_TYPE=DEBUG -G Ninja
cmake --build . --config <Debug> -j 8
```




