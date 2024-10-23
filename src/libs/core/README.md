Core C++ tools

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

#### Box
```C++
namespace box {

struct A {
    virtual ~A() = default;
    virtual int foo() const { return 0; }
};
struct B : public A {
    int foo() const { return 25; }
};
}  // namespace box

using box::A;
using box::B;
{
    Box<A> a{B{}};
    ASSERT_EQ(a->foo(), 25);
}

{
    Box<B> b;
    Box<A> a{std::move(b)};
    ASSERT_EQ(a->foo(), 25);
}

{
    Box<B> b;
    Box<A> a;
    a = std::move(b);
    ASSERT_EQ(a->foo(), 25);
}
```
#### Cow
```C++
Cow val = std::string{"Hello"};
auto val2 = val;
val = "Goodbye";
ASSERT_EQ(val2, "Hello");

```
#### Enum
```C++
Enum<int, double, std::string> val = 25;
ASSERT_TRUE(val.is<int>());
std::cout << val.as<int>();
val.match(
    [](int val)   { std::cout << "It is an int : " << val ; }
    [](double val){ std::cout << "It is an double : " << val ; }
    [](auto&& val){ std::cout << "It is something else " ; }
);
```

#### For constexpr
```C++
auto tuple = std::make_tuple(10, 10, 10);

for_constexpr<0, 3>([&](auto index) { std::get<index.value>(tuple) = index.value; });

ASSERT_EQ(std::get<0>(tuple), 0);
ASSERT_EQ(std::get<1>(tuple), 1);
ASSERT_EQ(std::get<2>(tuple), 2);
```

#### Result
```C++
enum class Error { Cause1, Cause2 };
auto task = []() -> Result<std::string, Error> { return Ok{"hello"}; };
auto result = task();
ASSERT_TRUE(result);
ASSERT_EQ(result.value(), "hello");

auto task = []() -> Result<std::string, Error> { return Fail{Error::Cause1}; };
auto result = task();
ASSERT_FALSE(result);
ASSERT_TRUE(result.is_error());
ASSERT_EQ(result.error(), Error::Cause1);
```

#### Scope guard
```C++
int counter = 0;
{
     EZ_ON_SCOPE_EXIT { ++counter; };
}
ASSERT_EQ(counter, 1);
```

#### Strong type
```C++
using Width = StrongType<int, struct WidthTag, mixin::NumberSemantic>;
using Height = StrongType<int, struct HeightTag, mixin::NumberSemantic>;
void resize(Widget& w, Width width, Height height);
Width width  = 10;
Height height = 20;

// The following will not compile !!
resize(w, height, width);  
width = height;
```

#### Tuple
```C++
Tuple tuple{1, std::string{"hello"}};
auto& [v1, v2] = tuple;
v1 = 10;
v2 = "toto";
ASSERT_EQ(tuple[constexpr_<0>], 10);
ASSERT_EQ(tuple[constexpr_<1>], "toto");
```

#### Rpl
Algorighm composition library
```C++
auto result = rpl::run(
    rpl::iota(1),
    rpl::transform([](int val) -> Option<int> { return (val % 2 == 0) ? Option<int>{val} : none; }),
    rpl::filter(),
    rpl::deref(),
    rpl::transform([](int val) { return val * val; }),
    rpl::take(3),
    rpl::to_vector()
);
ASSERT_EQ(result, (std::vector{2 * 2, 4 * 4, 6 * 6}));
```
