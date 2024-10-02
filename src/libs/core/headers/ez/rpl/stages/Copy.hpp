#pragma once

#include <ez/rpl/StageFactory.hpp>

#include <map>
#include <unordered_map>
#include <vector>

namespace ez::rpl {

namespace internal {

template <typename T>
void append(auto&& value, std::vector<T>& v)
{
    v.push_back(EZ_FWD(value));
}

template <typename K, typename V>
void append(auto&& pair, std::map<K, V>& map)
{
    map.insert({std::get<0>(EZ_FWD(pair)), std::get<1>(EZ_FWD(pair))});
}

template <typename K, typename V>
void append(auto&& pair, std::unordered_map<K, V>& map)
{
    map.insert({std::get<0>(EZ_FWD(pair)), std::get<1>(EZ_FWD(pair))});
}

template <typename ValueType, template <typename...> typename Container>
struct ContainerType {
    using Type = Container<ValueType>;
};

template <typename K, typename V, template <typename...> typename Container>
struct ContainerType<std::pair<K, V>, Container> {
    using Type = Container<std::remove_cvref_t<K>, std::remove_cvref_t<V>>;
};

template <typename K, typename V, template <typename...> typename Container>
struct ContainerType<Tuple<K, V>, Container> {
    using Type = Container<std::remove_cvref_t<K>, std::remove_cvref_t<V>>;
};

}  // namespace internal

template <typename InputType, template <typename...> typename Container>
struct CopyToImpl {
    using ValueType = std::remove_cvref_t<InputType>;

    using ContainerType = internal::ContainerType<ValueType, Container>::Type;

    using OutputType = ContainerType&&;

    ContainerType result;

    void process_incremental(InputType input, auto&&) { internal::append(EZ_FWD(input), result); }

    decltype(auto) flush_to(auto&& next) { return next.process_batch(std::move(result)); }
};

template <template <typename...> typename Container>
struct CopyTo {
    template <typename InputType>
    using Stage = CopyToImpl<InputType, Container>;
};

template <template <typename...> typename Container>
auto to()
{
    return make_factory<ProcessingMode::Incremental, ProcessingMode::Batch,
                        CopyTo<Container>::template Stage>();
}

inline auto to_vector() { return to<std::vector>(); }
inline auto to_map() { return to<std::map>(); }
inline auto to_unordered_map() { return to<std::unordered_map>(); }

}  // namespace ez::rpl
