#include "formatters.hpp"

#include <algorithm>
#include <vector>
#include <print>

using namespace std;

inline void rotate_array(vector<int>& nums, int k)
{
    const auto size = nums.size();
    k = k % size;

    if (!size or !k) return;

    while (k--) {
        int last = nums.back();
        for (uint i = 0; i < size - 1; ++i) { nums[size - i - 1] = nums[size - i - 2]; }
        nums.front() = last;
    }
}

int main()
{
    {
        vector nums{1, 2, 3, 4, 5, 6, 7};
        rotate_array(nums, 3);
        std::println("Result {}", nums);
    }

    {
        vector nums{-1, -100, 3, 99};
        rotate_array(nums, 2);
        std::println("Result {}", nums);
    }
}
