#include <ez/Print.hpp>
#include "formatters.hpp"

#include <algorithm>
#include <unordered_map>
#include <vector>

using namespace std;

class Solution {
public:
    bool canJump(vector<int>& nums)
    {
        int gas = 0;
        for (auto n : nums) {
            if (gas < 0)
                return false;
            else if (n > gas)
                gas = n;
            --gas;
        }
        return true;
    }
};

int main()
{
    auto test_case = [](vector<int> nums) {
        Solution sol;

        ez::println("------------");

        ez::println("Input  [{}] ", nums);
        bool element = sol.canJump(nums);
        ez::println("Result: {}", element);
    };

    test_case({2, 3, 1, 1, 4});
    test_case({3, 2, 1, 0, 4});
    test_case({1, 0, 8, 0});
}
