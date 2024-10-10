#include <gtest/gtest.h>

#include <ez/rpl/All.hpp>

#include <ez/Option.hpp>
#include <ez/Tuple.hpp>

#include <format>
#include <ranges>

using namespace ez;

template <typename... Ts>
void debug_type_list(TypeList<Ts...> tl)
{
    DebugTypes<EZ_TYPE_AT(tl, 0), EZ_TYPE_AT(tl, 1)>();
}

TEST(Rpl, stage_type)
{
    auto filter = rpl::filter([](int val) { return val > 2; });
    auto stage = filter.make(meta::type<int&>);
    unused(stage);
}

TEST(Rpl, get_chain_input_types)
{
    auto filter = rpl::filter([](int val) { return val > 2; });
    auto to_vector = rpl::to_vector();

    using Vector = std::vector<int>;

    {
        auto type_list = rpl::get_chain_input_types<rpl::ProcessingMode::Batch, Vector&,
                                                    decltype(filter), decltype(to_vector)>();
        static_assert(type_list == meta::type_list<int&, int&>);
    }

    {
        auto type_list = rpl::get_chain_input_types<rpl::ProcessingMode::Incremental, int&,
                                                    decltype(filter), decltype(to_vector)>();

        static_assert(type_list == meta::type_list<int&, int&>);
    }
}

TEST(Rpl, get_chain_input_types_view)
{
    auto filter = rpl::filter([](int val) { return val > 2; });
    auto to_vector = rpl::to_vector();

    using Input = std::ranges::iota_view<int>;

    using ValueType = decltype(*std::begin(std::declval<Input&&>()));

    {
        auto type_list = rpl::get_chain_input_types<rpl::ProcessingMode::Batch, Input&&,
                                                    decltype(filter), decltype(to_vector)>();

        static_assert(type_list == meta::type_list<int&&, int&&>);
    }
}

TEST(Rpl, chain_traits)
{
    auto filter = rpl::filter([](int val) { return val > 2; });
    auto to_vector = rpl::to_vector();

    using Vector = std::vector<int>;

    {
        using Traits = rpl::ChainTraits<rpl::ProcessingMode::Batch, Vector&, decltype(filter),
                                        decltype(to_vector)>;

        constexpr auto input_type_list = Traits::InputTypeList{};
        constexpr auto output_type = meta::type<Traits::OutputType>;

        static_assert(input_type_list == meta::type_list<int&, int&>);
        static_assert(output_type == meta::type<Vector&&>);
    }

    {
        using Traits = rpl::ChainTraits<rpl::ProcessingMode::Incremental, int&, decltype(filter),
                                        decltype(to_vector)>;

        constexpr auto input_type_list = Traits::InputTypeList{};
        constexpr auto output_type = meta::type<Traits::OutputType>;

        static_assert(input_type_list == meta::type_list<int&, int&>);
        static_assert(output_type == meta::type<Vector&&>);
    }
}

TEST(Rpl, chain_stages)
{
    auto filter = rpl::filter([](int val) { return val > 2; });
    auto to_vector = rpl::to_vector();

    using ChainStagesType =
        rpl::ChainStages<rpl::ProcessingMode::Incremental, int&, IndexSequenceFor<void, void>,
                         EZ_REMOVE_CVR_T(filter), EZ_REMOVE_CVR_T(to_vector)>;

    ChainStagesType chain{in_place, filter, to_vector};
    unused(chain);
}

TEST(Rpl, chain_incremental)
{
    auto filter = rpl::filter([](int val) { return val > 2; });
    auto to_vector = rpl::to_vector();

    using Chain =
        rpl::Chain<rpl::ProcessingMode::Incremental, int&, decltype(filter), decltype(to_vector)>;

    Chain chain{in_place, filter, to_vector};
    unused(chain);
}

TEST(Rpl, chain_batch)
{
    auto filter = rpl::filter([](int val) { return val > 2; });
    auto to_vector = rpl::to_vector();

    using Chain = rpl::Chain<rpl::ProcessingMode::Batch, std::vector<int>&, decltype(filter),
                             decltype(to_vector)>;

    Chain chain{in_place, filter, to_vector};
    unused(chain);
}

TEST(Rpl, make_chain)
{
    std::vector input{1, 2, 3, 4};

    auto chain = rpl::make_chain<std::vector<int>&>(rpl::filter([](int val) { return val > 2; }),
                                                    rpl::to_vector());

    using Chain = EZ_REMOVE_CVR_T(chain);
    using OutputType = typename Chain::OutputType;

    constexpr auto input_types = Chain::InputTypeList{};
    constexpr auto output_type = meta::type<OutputType>;

    static_assert(input_types.at(Index<0>{}) == meta::type<int&>);
    static_assert(input_types.at(Index<1>{}) == meta::type<int&>);
    static_assert(output_type == meta::type<std::vector<int>&&>);
}

TEST(Rpl, simple_run)
{
    std::vector input{4, 3, 2, 1, 0, -1, -2};

    // clang-format off
    auto result = rpl::run(
        input,
        rpl::filter([](int val) { return val > 2; }),
        rpl::transform([](int val) { return val * val; }),
        rpl::to_vector(),
        rpl::sort()
    );
    // clang-format on

    ASSERT_EQ(result, (std::vector{9, 16}));
}

TEST(Rpl, compose_incremental_input)
{
    auto f = rpl::filter([](int val) { return val > 2; });
    auto t = rpl::transform([](int val) { return val * val; });

    using C = rpl::Compose<int&, EZ_REMOVE_CVR_T(f), EZ_REMOVE_CVR_T(t)>;

    static_assert(C::input_processing_mode == rpl::ProcessingMode::Incremental);
    static_assert(C::output_processing_mode == rpl::ProcessingMode::Incremental);

    C cmp{f, t};

    int val;

    rpl::ChainTerminator terminator;

    static_assert(requires { cmp.process_incremental(val, terminator); });
}

TEST(Rpl, compose_batch_input)
{
    auto s = rpl::sort();
    auto t = rpl::transform([](int val) { return val * val; });

    using C = rpl::Compose<std::vector<int>&, EZ_REMOVE_CVR_T(s), EZ_REMOVE_CVR_T(t)>;

    using I = C::InputTypeList;

    static_assert(C::input_processing_mode == rpl::ProcessingMode::Batch);
    static_assert(C::output_processing_mode == rpl::ProcessingMode::Incremental);

    C cmp{s, t};

    std::vector<int> val;

    rpl::ChainTerminator terminator;

    static_assert(requires { cmp.process_batch(val, terminator); });
}

TEST(Rpl, compose)
{
    std::vector input{4, 3, 2, 1, 0, -1, -2};

    auto pipeline = rpl::compose(rpl::filter([](int val) { return val > 2; }), rpl::to_vector());
    auto result = rpl::run(input, pipeline);

    ASSERT_EQ(result, (std::vector{4, 3}));
}

TEST(Rpl, parallel)
{
    std::vector input{1, 2};

    auto pipeline = rpl::parallel(rpl::to_vector(), rpl::to_vector());
    auto result = rpl::run(input, pipeline);
    ASSERT_EQ(result, Tuple(std::vector{1, 2}, std::vector{1, 2}));
}

TEST(Rpl, iota_pipeline)
{
    // clang-format off
    auto result = rpl::run(
        rpl::iota(1),
        rpl::transform([](int val) -> Option<int> { return (val % 2 == 0) ? Option<int>{val} : none; }),
        rpl::filter(),
        rpl::deref(),
        rpl::transform([](int val) { return val * val; }),
        rpl::take(3),
        rpl::to_vector()
    );
    // clang-format on

    ASSERT_EQ(result, (std::vector{2 * 2, 4 * 4, 6 * 6}));
}

TEST(Rpl, min_max_count)
{
    // clang-format off
    auto result = rpl::run(
        rpl::iota(1, 10),
        rpl::parallel(rpl::min(), rpl::max(), rpl::count())
        );
    // clang-format on

    ASSERT_EQ(result, (Tuple{1, 9, 9}));
}

TEST(Rpl, apply)
{
    // clang-format off
    auto result = rpl::run(
        rpl::iota(1, 3),
        rpl::transform([](auto val ){return Tuple {val, val + 1, val+2};}),
        rpl::apply([](int v1, int v2, int v3){ return v1 * v2 * v3;}),
        rpl::to_vector()
        );
    // clang-format on

    ASSERT_EQ(result, (std::vector{6, 24}));
}

TEST(Rpl, enumerate)
{
    // clang-format off
    auto result = rpl::run(
        rpl::iota(1, 3),
        rpl::enumerate(),
        rpl::apply([](size_t index, auto&&){ return index;}),
        rpl::to_vector()
        );
    // clang-format on

    ASSERT_EQ(result, (std::vector<size_t>{0, 1}));
}

TEST(Rpl, to_map)
{
    // clang-format off
    auto result = rpl::run(
        rpl::iota(1, 3),
        rpl::enumerate(),
        rpl::to_map()
    );
    // clang-format on

    ASSERT_EQ(result, (std::map<size_t, int>{{0, 1}, {1, 2}}));
}

TEST(Rpl, to_unordered_map)
{
    // clang-format off
    auto result = rpl::run(
        rpl::iota(1, 3),
        rpl::enumerate(),
        rpl::to_unordered_map()
        );
    // clang-format on

    ASSERT_EQ(result, (std::unordered_map<size_t, int>{{0, 1}, {1, 2}}));
}

TEST(Rpl, reorder)
{
    // clang-format off
    auto result = rpl::run(
        rpl::iota(1, 3),
        rpl::enumerate(),
        rpl::reorder<1,0>(),
        rpl::apply([](int&&, size_t index){ return index;}),
        rpl::to_vector()
        );
    // clang-format on

    ASSERT_EQ(result, (std::vector<size_t>{0, 1}));
}

TEST(Rpl, get)
{
    // clang-format off
    auto result = rpl::run(
        rpl::iota(1, 3),
        rpl::enumerate(),
        rpl::reorder<1,0>(),
        rpl::get<1>(),
        rpl::to_vector()
        );
    // clang-format on

    ASSERT_EQ(result, (std::vector<size_t>{0, 1}));
}
