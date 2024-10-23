## ez-cpp
C++ tools and experimentation playground.

### Libraries
#### ez.core
[Core C++ tools](src/libs/core/README.md)

#### ez.async
[C++ coroutine library](src/libs/async/README.md)

#### ez.async
[Asynchrounous RPC framework library](src/libs/rpc/README.md)

#### ez.flow
Interpreter and virtual machine for the experimental flow async programming language.
### Build instructions
#### Requirements
* A C++20 compliant compiler
* CMake 3.2 or more recent
* Conan 2.x 

#### Building with CMake

```
mkdir build
cd build
conan install -of . --build missing -s build_type=<Debug> -r conan-center --profile <msvc2022> ../
cmake .. -DCMAKE_TOOLCHAIN_FILE="build/generators/conan_toolchain.cmake"
cmake -DCMAKE_BUILD_TYPE=<Debug> ..
cmake --build . --config <Debug> -j 8
```




