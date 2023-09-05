module;

#include <memory>

export module ez.cow;

export namespace ez {

template <typename T>
class Cow {
public:
private:
    std::shared_ptr<T> m_value;
};

}  // namespace ez
