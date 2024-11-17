#include <ez/Print.hpp>
#include "formatters.hpp"

#include <algorithm>
#include <vector>

using namespace std;

inline int max_profit(vector<int>& prices)
{
    int profit = 0;
    auto iter = prices.begin();
    const auto end = prices.end();

    int min_price = *iter;

    while (++iter != end) {
        min_price = std::min(min_price, *iter);
        profit = std::max(profit, *iter - min_price);
    }

    return profit;
}

int main()
{
    {
        vector nums{7, 3, 5, 1, 6, 4};
        auto result = max_profit(nums);
        ez::println("Result [{}] -> {}", nums, result);
    }

    {
        vector nums{7, 6, 4, 3, 1};
        auto result = max_profit(nums);
        ez::println("Result [{}] -> {}", nums, result);
    }
}
