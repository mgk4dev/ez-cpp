#include <ez/Print.hpp>
#include "formatters.hpp"

#include <algorithm>
#include <vector>

using namespace std;

inline int remove_duplicates(vector<int>& nums)
{
    auto iter = nums.begin();
    auto end = nums.end();

    if (iter == end) { return 0; }

    int count = 0;

    while (iter != end) {
        auto next = std::upper_bound(iter + 1, end, *iter);
        auto distance = std::distance(iter, next);

        if (distance > 2) {
            std::shift_left(iter + 1, end, distance - 2);
            count += 2;
            iter += 2;
            end -= (distance - 2);
        }

        else {
            count += 1;
            ++iter;
        }

    }

    return count;
}

int main()
{
    auto test_case = [](vector<int> nums) {
        ez::println("------------");

        ez::println("Input  [{}] ", nums);
        int count = remove_duplicates(nums);
        ez::println("Result [{}] | {}", nums, count);
    };

    test_case({1, 1, 1, 2, 2, 3});
    test_case({0, 0, 1, 1, 1, 1, 2, 3, 3});
    test_case({1, 1, 1});
    test_case({1, 2, 3});
    test_case({1, 1, 1, 2});
}
