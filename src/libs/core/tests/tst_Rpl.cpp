#include <gtest/gtest.h>

#include <ez/rpl/All.hpp>

#include <ez/Lambda.hpp>
#include <ez/Option.hpp>
#include <ez/Tuple.hpp>

#include <format>
#include <ranges>

using namespace ez;

TEST(Rpl, stage_type)
{
    using namespace ez::lambda::args;
    auto filter = rpl::filter(arg1 > 2);
    auto stage = filter.make(meta::type<int&>);
    unused(stage);
}

TEST(Rpl, get_pipeline_input_types)
{
    using namespace ez::lambda::args;

    auto filter = rpl::filter(arg1 > 2);
    auto to_vector = rpl::to_vector();

    using Vector = std::vector<int>;

    {
        auto type_list = rpl::get_pipeline_input_types<rpl::ProcessingMode::Batch, Vector&,
                                                       decltype(filter), decltype(to_vector)>();
        static_assert(type_list == meta::type_list<int&, int&>);
    }

    {
        auto type_list = rpl::get_pipeline_input_types<rpl::ProcessingMode::Incremental, int&,
                                                       decltype(filter), decltype(to_vector)>();

        static_assert(type_list == meta::type_list<int&, int&>);
    }
}

TEST(Rpl, get_pipeline_input_types_view)
{
    using namespace ez::lambda::args;

    auto filter = rpl::filter(arg1 > 25);
    auto to_vector = rpl::to_vector();

    using Input = std::ranges::iota_view<int>;

    {
        auto type_list = rpl::get_pipeline_input_types<rpl::ProcessingMode::Batch, Input&&,
                                                       decltype(filter), decltype(to_vector)>();

        static_assert(type_list == meta::type_list<int&&, int&&>);
    }
}

TEST(Rpl, pipeline_traits)
{
    using namespace ez::lambda::args;

    auto filter = rpl::filter(arg1 > 2);
    auto to_vector = rpl::to_vector();

    using Vector = std::vector<int>;

    {
        using Traits = rpl::PipelineTraits<rpl::ProcessingMode::Batch, Vector&, decltype(filter),
                                           decltype(to_vector)>;

        constexpr auto input_type_list = Traits::InputTypeList{};
        constexpr auto output_type = meta::type<Traits::OutputType>;

        static_assert(input_type_list == meta::type_list<int&, int&>);
        static_assert(output_type == meta::type<Vector&&>);
    }

    {
        using Traits = rpl::PipelineTraits<rpl::ProcessingMode::Incremental, int&, decltype(filter),
                                           decltype(to_vector)>;

        constexpr auto input_type_list = Traits::InputTypeList{};
        constexpr auto output_type = meta::type<Traits::OutputType>;

        static_assert(input_type_list == meta::type_list<int&, int&>);
        static_assert(output_type == meta::type<Vector&&>);
    }
}

TEST(Rpl, pipeline_stages)
{
    using namespace ez::lambda::args;

    auto filter = rpl::filter(arg1 > 2);
    auto to_vector = rpl::to_vector();

    using PipelineStages =
        rpl::PipelineStages<rpl::ProcessingMode::Incremental, int&, IndexSequenceFor<void, void>,
                            EZ_REMOVE_CVR_T(filter), EZ_REMOVE_CVR_T(to_vector)>;

    PipelineStages pipeline{in_place, filter, to_vector};
    unused(pipeline);
}

TEST(Rpl, pipeline_incremental)
{
    using namespace ez::lambda::args;

    auto filter = rpl::filter(arg1 > 2);
    auto to_vector = rpl::to_vector();

    using Pipeline = rpl::Pipeline<rpl::ProcessingMode::Incremental, int&, decltype(filter),
                                   decltype(to_vector)>;

    Pipeline pipeline{in_place, filter, to_vector};
    unused(pipeline);
}

TEST(Rpl, pipeline_batch)
{
    using namespace ez::lambda::args;

    auto filter = rpl::filter(arg1 > 10);
    auto to_vector = rpl::to_vector();

    using Pipeline = rpl::Pipeline<rpl::ProcessingMode::Batch, std::vector<int>&, decltype(filter),
                                   decltype(to_vector)>;

    Pipeline pipeline{in_place, filter, to_vector};
    unused(pipeline);
}

TEST(Rpl, make_pipeline)
{
    std::vector input{1, 2, 3, 4};

    auto pipeline = rpl::make_pipeline<std::vector<int>&>(
        rpl::filter([](int val) { return val > 2; }), rpl::to_vector());

    using Pipeline = EZ_REMOVE_CVR_T(pipeline);
    using OutputType = typename Pipeline::OutputType;

    constexpr auto input_types = Pipeline::InputTypeList{};
    constexpr auto output_type = meta::type<OutputType>;

    static_assert(input_types.at(Index<0>{}) == meta::type<int&>);
    static_assert(input_types.at(Index<1>{}) == meta::type<int&>);
    static_assert(output_type == meta::type<std::vector<int>&&>);
}

TEST(Rpl, simple_run)
{
    using namespace ez::lambda::args;

    std::vector input{4, 3, 2, 1, 0, -1, -2};

    // clang-format off
    auto result = rpl::run(
        input,
        rpl::filter(arg1 > 2),
        rpl::transform(arg1 * arg1),
        rpl::to_vector(),
        rpl::sort()
    );
    // clang-format on

    ASSERT_EQ(result, (std::vector{9, 16}));
}

TEST(Rpl, compose_incremental_input)
{
    using namespace ez::lambda::args;

    auto f = rpl::filter(arg1 > 2);
    auto t = rpl::transform(arg1 * arg1);

    using C = rpl::Compose<int&, EZ_REMOVE_CVR_T(f), EZ_REMOVE_CVR_T(t)>;

    static_assert(C::input_processing_mode == rpl::ProcessingMode::Incremental);
    static_assert(C::output_processing_mode == rpl::ProcessingMode::Incremental);

    C cmp{f, t};

    int val;

    rpl::PipelineTerminator terminator;

    static_assert(requires { cmp.process_incremental(val, terminator); });
}

TEST(Rpl, compose_batch_input)
{
    using namespace ez::lambda::args;

    auto s = rpl::sort();
    auto t = rpl::transform(arg1 * arg1);

    using C = rpl::Compose<std::vector<int>&, EZ_REMOVE_CVR_T(s), EZ_REMOVE_CVR_T(t)>;

    static_assert(C::input_processing_mode == rpl::ProcessingMode::Batch);
    static_assert(C::output_processing_mode == rpl::ProcessingMode::Incremental);

    C cmp{s, t};

    std::vector<int> val;

    rpl::PipelineTerminator terminator;

    static_assert(requires { cmp.process_batch(val, terminator); });
}

TEST(Rpl, compose)
{
    using namespace ez::lambda::args;

    std::vector input{4, 3, 2, 1, 0, -1, -2};

    auto pipeline = rpl::compose(rpl::filter(arg1 > 2), rpl::to_vector());
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
    using namespace ez::lambda::args;

    // clang-format off
    auto result = rpl::run(
        rpl::iota(1),
        rpl::transform([](int val) -> Option<int> { return (val % 2 == 0) ? Option<int>{val} : none; }),
        rpl::filter(),
        rpl::deref(),
        rpl::transform( arg1 * arg1),
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
    using namespace ez::lambda::args;

    // clang-format off
    auto result = rpl::run(
        rpl::iota(1, 3),
        rpl::transform([](auto val ){return Tuple {val, val + 1, val+2};}),
        rpl::apply(arg1 * arg2 * arg3),
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
        rpl::get<0>(),
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
        rpl::get<1>(),
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

TEST(Rpl, skip_duplicates)
{
    // clang-format off
    auto result = rpl::run(
        std::vector{1,1,1,1,2,2,2,3,4,4},
        rpl::skip_duplicates(),
        rpl::to_vector()
    );
    // clang-format on

    ASSERT_EQ(result, (std::vector{1, 2, 3}));
}

TEST(Rpl, for_each)
{
    using namespace ez::lambda::args;

    std::vector<int> result;
    // clang-format off
    rpl::run(
        rpl::iota(1),
        rpl::transform(arg1 * 2),
        rpl::take(3),
        rpl::for_each([&](auto&& val) {result.push_back(val);})
    );
    // clang-format on

    ASSERT_EQ(result, (std::vector{2, 4, 6}));
}

TEST(Rpl, accumulate)
{
    // clang-format off
    auto result = rpl::run(
        rpl::iota(1,4),
        rpl::accumulate<int>()
        );
    // clang-format on

    ASSERT_EQ(result, 1 + 2 + 3);
}

TEST(Rpl, filter_args_ez_tuple)
{
    using namespace ez::lambda::args;

    // clang-format off
    auto result = rpl::run(
        rpl::iota(1,4),
        rpl::transform([](int val ) {return Tuple{val, val};}),
        rpl::filter(arg1 == arg2),
        rpl::count()
        );
    // clang-format on

    ASSERT_EQ(result, 3);
}

TEST(Rpl, filter_args_std_tuple)
{
    using namespace ez::lambda::args;

    // clang-format off
    auto result = rpl::run(
        rpl::iota(1,4),
        rpl::transform([](int val ) {return std::tuple{val, val};}),
        rpl::filter(arg1 == arg2),
        rpl::count()
        );
    // clang-format on

    ASSERT_EQ(result, 3);
}

TEST(Rpl, filter_args_std_pair)
{
    using namespace ez::lambda::args;

    // clang-format off
    auto result = rpl::run(
        rpl::iota(1,4),
        rpl::transform([](int val ) {return std::pair{val, val};}),
        rpl::filter(arg1 == arg2),
        rpl::count()
        );
    // clang-format on

    ASSERT_EQ(result, 3);
}
