module;

#include <memory>

export module ez.box;

export namespace ez {

template <typename T>
class Box {
public:
private:
    std::unique_ptr<T> m_value;
};

}  // namespace ez
