#include <ez/Print.hpp>
#include "formatters.hpp"

#include <algorithm>
#include <vector>

using namespace std;

inline int majority_element(vector<int>& nums)
{
    uint count = 0;
    int majoritiy_element = 0;

    for (int val : nums) {
        if (count == 0) {
            majoritiy_element = val;
            ++count;
        }
        else if (majoritiy_element == val) {++count;}
        else {--count;}
    }

    return majoritiy_element;
}

int main()
{
    auto test_case = [](vector<int> nums) {
        ez::println("------------");

        ez::println("Input  [{}] ", nums);
        int element = majority_element(nums);
        ez::println("Result: {}", element);
    };

    test_case({3, 2, 3});
    test_case({2, 2, 1, 1, 1, 2, 2});
    test_case({1, 1, 2, 2, 2});
    test_case({1, 2, 1, 2, 1});
    test_case({0, 1, 1, 1, 2});
}
