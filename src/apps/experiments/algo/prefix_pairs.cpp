
#include <algorithm>
#include <map>
#include <memory>
#include <print>
#include <vector>

using namespace std;

class Trie {
    struct Node {
        map<char, unique_ptr<Node>> children;
        bool is_last_char = false;
    };

    Node root;

    static void words_in(string& word, const Node& node, vector<string>& result)
    {
        for (const auto& [c, child] : node.children) {
            word.push_back(c);

            if (child->is_last_char) { result.push_back(word); }
            words_in(word, *child.get(), result);

            word.pop_back();
        }
    }

public:
    void insert(string_view str)
    {
        auto node = &root;
        for (char c : str) {
            if (!node->children.contains(c)) node->children[c] = make_unique<Node>();
            node = node->children[c].get();
        }
        node->is_last_char = true;
    }

    vector<string> words_containing(string prefix) const
    {
        vector<string> result;

        auto node = &root;
        for (char c : prefix) {
            if (node->children.contains(c)) { node = node->children.at(c).get(); }
            else {
                return {};
            }
        }

        words_in(prefix, *node, result);

        return result;
    }
};

inline auto prefix_pairs(vector<string> words)
{
    Trie trie;

    for (auto&& word : words) { trie.insert(word); }

    vector<pair<string, string>> result;

    for (auto&& word : words) {
        for (auto match : trie.words_containing(word)) result.emplace_back(word, match);
    }

    return result;
}

int main()
{
    auto result = prefix_pairs({
        "a",
        "aa",
        "aaa",
        "aaab",
        "b",
        "ba",
        "bab",
        "bb",
        "bbab",
    });

    for (auto& item : result) std::println("({}, {})", item.first, item.second);
}
