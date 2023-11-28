## ez-cpp
C++ tools and experimentation playground.

### Components
#### ez.core
Core C++ tools

#### ez.async
C++ coroutine library

#### ez.flow
Interpreter and virtual machine for the experimental flow async programming language.
### Build instructions
#### Requirements
* A C++23 compliant compiler
* CMake 3.27 or more recent
* Conan 1.60 or more recent

#### Building with CMake

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=(Debug|Release) ..
make
```


