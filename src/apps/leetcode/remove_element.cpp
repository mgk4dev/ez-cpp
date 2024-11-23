#include "formatters.hpp"

#include <algorithm>
#include <print>
#include <vector>

using namespace std;

inline int remove_element(vector<int>& nums, int val)
{
    auto iter = std::partition(nums.begin(), nums.end(), [val](int v) { return v != val; });
    return std::distance(nums.begin(), iter);
}

int main()
{
    {
        vector nums{3, 2, 2, 3};
        int count = remove_element(nums, 3);
        std::println("Result {} | {}", nums, count);
    }

    {
        vector nums{0, 1, 2, 2, 3, 0, 4, 2};
        int count = remove_element(nums, 2);
        std::println("Result {} | {}", nums, count);
    }
}
