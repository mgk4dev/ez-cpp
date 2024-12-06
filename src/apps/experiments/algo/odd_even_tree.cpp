
#include <algorithm>
#include <deque>
#include <memory>
#include <print>

using namespace std;

struct Node {
    int data = 0;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
};

inline bool is_odd_even(const Node& tree)
{
    using NodeCollection = std::deque<const Node*>;
    NodeCollection nodes;
    NodeCollection tmp_nodes;

    auto try_add = [](NodeCollection& nodes, const Node* node) {
        if (node) nodes.push_back(node);
    };

    try_add(nodes, tree.left.get());
    try_add(nodes, tree.right.get());

    bool ascending_order = true;
    bool is_sorted = true;
    const auto projection = [](auto node) { return node->data; };

    while (!nodes.empty()) {
        ascending_order = !ascending_order;

        if (ascending_order)
            is_sorted = std::ranges::is_sorted(nodes, std::less{}, projection);
        else
            is_sorted = std::ranges::is_sorted(nodes, std::greater{}, projection);

        if (!is_sorted) return false;

        for (auto node : nodes) {
            try_add(tmp_nodes, node->left.get());
            try_add(tmp_nodes, node->right.get());
        }

        std::swap(nodes, tmp_nodes);
        tmp_nodes.clear();
    }

    return true;
}

Node make_tree()
{
    Node root{
        1,
        std::make_unique<Node>(10, std::make_unique<Node>(3)),
        std::make_unique<Node>(
            4,
            std::make_unique<Node>(7),
            std::make_unique<Node>(9))
    };

    return root;
}

int main()
{
    auto tree = make_tree();
    std::println("Is even odd tree : {}", is_odd_even(tree));
}
