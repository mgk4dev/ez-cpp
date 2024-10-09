#include <gtest/gtest.h>

#include <ez/rpl/All.hpp>

#include <ez/Tuple.hpp>

#include <format>

using namespace ez;

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

        std::cout << typeid(type_list.at(Index<0>())).name() << std::endl;
        std::cout << typeid(type_list.at(Index<1>())).name() << std::endl;

        static_assert(type_list == meta::type_list<int&, int&>);
    }

    {
        auto type_list = rpl::get_chain_input_types<rpl::ProcessingMode::Incremental, int&,
                                                    decltype(filter), decltype(to_vector)>();

        static_assert(type_list == meta::type_list<int&, int&>);
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

TEST(Rpl, compose)
{
    std::vector input{4, 3, 2, 1, 0, -1, -2};

    auto filter_transform = rpl::compose(rpl::filter([](int val) { return val > 2; }),
                                         rpl::transform([](int val) { return val * val; }));

    static_assert(filter_transform.input_processing_mode<int&> == rpl::ProcessingMode::Incremental);
    static_assert(filter_transform.output_processing_mode<int&> == rpl::ProcessingMode::Incremental);

     auto chain =
         rpl::make_chain<std::vector<int>&>(filter_transform, rpl::to_vector(), rpl::sort());

    // using I = typename decltype(chain)::InputTypeList;

    // DebugTypes<I>();

    // // clang-format off
    //  auto result = rpl::run(
    //     input,
    //     filter_transform,
    //     rpl::to_vector(),
    //     rpl::sort()
    // );
    // // clang-format on

    // ASSERT_EQ(result, (std::vector{9, 16}));
}
