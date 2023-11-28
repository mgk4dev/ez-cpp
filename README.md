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
cmake build .
```


### Snippets

#### Atomic

```C++
    Atomic<std::vector<int>> vec;
    const size_t size = 10000;

    auto producer = [&vec](size_t count) {
        for (size_t i = 0; i < count; ++i) { vec->push_back(i); }
    };

    auto consumer = [&vec](size_t max) {
        size_t total = 0;

        while (total != max) {
            vec.edit([&](auto& vec) {
                if (!vec.empty()) {
                    vec.pop_back();
                    ++total;
                }
            });
        }
    };

    auto p1 = std::async(std::launch::async, producer, size);
    auto p2 = std::async(std::launch::async, producer, size);
    auto c = std::async(std::launch::async, consumer, size * 2);

    p1.wait();
    p2.wait();
    c.wait();

    ASSERT_TRUE(vec->empty());
```