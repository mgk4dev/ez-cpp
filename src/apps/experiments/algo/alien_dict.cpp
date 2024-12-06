#include <algorithm>
#include <map>
#include <print>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

struct Graph {
    using List = unordered_set<char>;
    unordered_map<char, List> connections;

    void connect(char lhs, char rhs) { connections[lhs].insert(rhs); }

    void print() const
    {
        for (auto& [from, l] : connections) {
            for (auto to : l) { println("{} -> {}", from, to); }
        }
    }
};

void connect_chars(Graph& graph, string_view lhs, string_view rhs)
{
    while (!lhs.empty() && !rhs.empty()) {
        if (lhs.front() == rhs.front()) {
            lhs.remove_prefix(1);
            rhs.remove_prefix(1);
        }
        else {
            graph.connect(lhs.front(), rhs.front());
            return;
        }
    }
}

void visit(const Graph& graph,
           char node,
           unordered_map<char, uint>& nodes_depth,
           uint current_depth)
{
    if (!graph.connections.contains(node)) return;

    ++current_depth;

    for (auto child : graph.connections.at(node)) {
        if (!nodes_depth.contains(child) || nodes_depth[child] < current_depth) {
            nodes_depth[child] = current_depth;
            visit(graph, child, nodes_depth, current_depth);
        }
    }
}

string determine_alphabet(const vector<string>& words)
{
    Graph graph;

    for (auto i = words.begin() + 1; i < words.end(); ++i) { connect_chars(graph, *(i - 1), *i); }

    graph.print();

    unordered_map<char, uint> nodes_depth;
    nodes_depth[words.front().front()] = 0;
    visit(graph, words.front().front(), nodes_depth, 0);

    for (auto [c, d] : nodes_depth) println("{} : {}", c, d);

    map<uint, char> ordered_chars;

    string result;

    for (auto [c, _] : nodes_depth) result.push_back(c);

    ranges::sort(result, [&](char lhs, char rhs) { return nodes_depth[lhs] < nodes_depth[rhs]; });

    return result;
}

int main()
{
    auto result = determine_alphabet({"pbb", "bpku", "bpkb", "kbp", "kbu"});
    println("Result : {}", result);
}
