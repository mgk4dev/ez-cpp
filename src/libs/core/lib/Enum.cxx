module;

#include <variant>

export module ez.enum_;

export namespace ez {

template <typename... Ts>
class Enum : public std::variant<Ts...> {
public:
    using std::variant<Ts...>::variant;
};

}  // namespace ez
