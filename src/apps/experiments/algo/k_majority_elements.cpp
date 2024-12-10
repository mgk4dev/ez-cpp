
#include "formatters.hpp"

#include <algorithm>
#include <map>
#include <print>
#include <queue>
#include <ranges>
#include <vector>

using namespace std;

vector<int> k_most_frequent(const vector<int>& numbers, unsigned int k)
{
    map<int, size_t> most_frequenet_count;

    for (int number : numbers) {
        if (most_frequenet_count.contains(number)) {
            most_frequenet_count[number]++;
            continue;
        }

        if (most_frequenet_count.size() < k) {
            most_frequenet_count[number] = 1;
            continue;
        }

        map<int, size_t> tmp;

        for (auto [elem, count] : most_frequenet_count) {
            if (count > 1) tmp.insert({elem, count - 1});
        }

        if (most_frequenet_count.size() < k) { most_frequenet_count[number] = 1; }

        most_frequenet_count = std::move(tmp);
    }

    vector<int> result;
    for (auto [n, _] : most_frequenet_count) { result.push_back(n); }
    return result;
}

int main()
{
    auto test_case = [](vector<int> elements, unsigned int k) {
        auto most_frequent = k_most_frequent(elements, k);
        println("{} most frequent : {}", k, most_frequent);
    };

    test_case({1, 1, 1, 2, 2, 3}, 2);
    test_case({1}, 1);
    test_case({1, 1, 2, 2, 3, 3}, 3);
}
