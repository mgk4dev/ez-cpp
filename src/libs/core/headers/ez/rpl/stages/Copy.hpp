#pragma once

#include <ez/rpl/StageFactory.hpp>

#include <ez/Utils.hpp>

#include <functional>
#include <type_traits>

namespace ez::rpl {

template <template <typename...> typename Collection, typename... CollectionParams>
struct CollectionTemplate {
    template <typename T>
    using Type = Collection<T, CollectionParams...>;
};

template <typename InputType, typename CollectionTemplateParam, typename Appender>
struct ToCollection {
    EZ_RPL_STAGE_INFO(ProcessingMode::Incremental, ProcessingMode::Batch)

    using Collection = typename CollectionTemplateParam::template Type<std::decay_t<InputType>>;
    using OutputType = Collection&&;

    Collection result;
    Appender appender;

    template <typename... Args>
    explicit ToCollection(Appender appender, Args&&... args)
        : appender(std::move(appender)), result(std::forward<Args>(args)...)
    {
    }

    void process_incremental(InputType input, auto&&)
    {
        appender(result, static_cast<InputType>(input));
    }

    decltype(auto) end(auto&& next) { return next.process_batch(std::move(result)); }
};

template <template <typename...> typename Collection,
          typename... CollectionParams,
          typename Appender,
          typename... Args>
auto to_collection(Appender appender, Args&&... args)
{
    return make_factory<ToCollection, CollectionTemplate<Collection, CollectionParams...>,
                        Appender>(std::move(appender), std::forward<Args>(args)...);
}

inline auto to_vector()
{
    return to_collection<std::vector>(
        [](auto& vector, auto&& value) { vector.push_back(std::forward<decltype(value)>(value)); });
}

}  // namespace ez::rpl
