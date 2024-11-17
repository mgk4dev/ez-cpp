#include <algorithm>
#include <ez/Print.hpp>
#include <vector>

#include "formatters.hpp"

using namespace std;

// [1, 2, 3, 0, 0] [2, 5, 6]
//     ^
// [1, _, 2, 3, 0]
// [1, 2, 2, 3, 0] [5, 6]

inline void merge_sorted_arrays(vector<int>& nums1, int m, const vector<int>& nums2, int n)
{
    if (n == 0) return;

    nums1.resize(n + m);

    if (m == 0) {
        copy(nums2.begin(), nums2.end(), nums1.begin());
        return;
    }

    auto begin1 = nums1.begin();
    auto end1 = begin1 + m;
    auto begin2 = nums2.begin();

    while (begin2 != nums2.end()) {

        auto insertion_point = std::lower_bound(begin1, end1, *begin2);
        std::shift_right(insertion_point, end1 + 1, 1);
        *insertion_point = *begin2;
        begin1 = insertion_point + 1;
        ++end1;
        ++begin2;
    }
}

int main()
{
    {
        vector nums1{1, 2, 3, 0, 0, 0};
        vector nums2{2, 5, 6};
        merge_sorted_arrays(nums1, 3, nums2, 3);
        ez::println("Result {}", nums1);
    }

    {
        vector nums1{1};
        vector<int> nums2{};
        merge_sorted_arrays(nums1, 1, nums2, 0);
        ez::println("Result {}", nums1);
    }

    {
        vector nums1{0};
        vector<int> nums2{1};
        merge_sorted_arrays(nums1, 0, nums2, 1);
        ez::println("Result {}", nums1);
    }
}
