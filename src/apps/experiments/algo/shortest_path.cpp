
#include <algorithm>
#include <print>
#include <ranges>
#include <unordered_map>
#include <unordered_set>

#include "formatters.hpp"

struct Graph {
    using Node = char;
    using Distance = int;

    struct Connection {
        Node destination;
        Distance distance;
    };

    using NodeSet = std::unordered_set<Node>;
    using ConnectionMap = std::unordered_multimap<Node, Connection>;

    NodeSet nodes;
    ConnectionMap connections;

    void connect_direct(Node node1, Node node2, Distance d)
    {
        connections.insert({node1, Connection{node2, d}});

        nodes.insert(node1);
        nodes.insert(node2);
    }

    void connect_cyclic(Node node1, Node node2, Distance d)
    {
        connections.insert({node1, Connection{node2, d}});
        connections.insert({node2, Connection{node1, d}});

        nodes.insert(node1);
        nodes.insert(node2);
    }

    auto connections_of(Node node) const
    {
        auto begin_end = connections.equal_range(node);
        return std::ranges::subrange(std::get<0>(begin_end), std::get<1>(begin_end));
    }
};

void print(const Graph& graph)
{
    std::println("Graph [");

    for (auto& [from, connection] : graph.connections)
        std::println("   {} -> {} : {}", from, connection.destination, connection.distance);

    std::println("]");
}

// V: vertex/node count
// E: edge/connection count
// Time : O(V²) | O((V+E) x log(V))
// Space: O(V)
std::vector<Graph::Node> dijkstra_shortest_path(const Graph& graph,
                                                Graph::Node source_node,
                                                Graph::Node destination_node)
{
    std::unordered_map<Graph::Node, Graph::Distance> distance_map;
    std::unordered_map<Graph::Node, Graph::Node> predecessor_map;
    Graph::NodeSet unvisited = graph.nodes;

    distance_map[source_node] = {0};

    Graph::Node current_node = source_node;

    while (unvisited.contains(destination_node)) {
        for (auto connection : graph.connections_of(current_node)) {
            auto destination = connection.second.destination;
            auto distance = connection.second.distance;

            auto total_distance = distance + distance_map[current_node];

            if (!distance_map.contains(destination) || distance_map[destination] > total_distance) {
                distance_map[destination] = total_distance;
                predecessor_map[destination] = current_node;
            }
        }

        unvisited.erase(current_node);

        {
            Graph::Distance min_distance = std::numeric_limits<Graph::Distance>::max();

            for (auto& [node, distance] : distance_map) {
                if (distance < min_distance && unvisited.contains(node)) {
                    min_distance = distance;
                    current_node = node;
                }
            }
        }

        unvisited.erase(current_node);
    }

    std::vector<Graph::Node> result;

    {
        Graph::Node current = destination_node;
        result.push_back(current);
        while (current != source_node) {
            current = predecessor_map.at(current);
            result.push_back(current);
        }

        std::ranges::reverse(result);
    }

    return result;
}

// V: vertex/node count
// E: edge/connection count
// Time : O(V*E)
// Space: O(V)
std::vector<Graph::Node> bellman_ford_shortest_path(const Graph& graph,
                                                    Graph::Node source_node,
                                                    Graph::Node destination_node)

{
    std::unordered_map<Graph::Node, Graph::Distance> distance_map;
    std::unordered_map<Graph::Node, Graph::Node> predecessor_map;

    distance_map[source_node] = {0};

    while (true) {
        bool updated = false;
        for (auto& [from, to] : graph.connections) {
            if (distance_map.contains(from)) {
                if (!distance_map.contains(to.destination) or
                    (distance_map[to.destination] > distance_map[from] + to.distance)) {
                    distance_map[to.destination] = distance_map[from] + to.distance;
                    predecessor_map[to.destination] = from;
                    updated = true;
                }
            }
        }

        if (!updated) break;
    }

    std::vector<Graph::Node> result;

    {
        Graph::Node current = destination_node;
        result.push_back(current);
        while (current != source_node) {
            current = predecessor_map.at(current);
            result.push_back(current);
        }

        std::ranges::reverse(result);
    }

    return result;
}

void run_shortest_path(auto algorithm,
                       const Graph& graph,
                       Graph::Node source,
                       Graph::Node destination)
{
    auto path = algorithm(graph, source, destination);

    std::println("--- Path from {} to {}", source, destination);
    for (auto node : path) std::print("{} ", node);
    std::println();
}

int main()
{
    // {
    //     Graph graph;

    //     graph.connect_direct('A', 'B', 5);
    //     graph.connect_direct('A', 'C', 35);
    //     graph.connect_direct('A', 'D', 40);

    //     graph.connect_direct('B', 'E', 25);
    //     graph.connect_direct('B', 'D', 20);

    //     graph.connect_direct('C', 'E', 30);
    //     graph.connect_direct('C', 'F', 30);

    //     graph.connect_direct('E', 'D', 45);
    //     graph.connect_direct('E', 'F', 25);

    //     graph.connect_direct('D', 'F', 20);

    //     print(graph);

    //     run_shortest_path(&dijkstra_shortest_path, graph, 'A', 'F');
    // }

    {
        Graph graph;

        graph.connect_direct('A', 'B', 5);
        graph.connect_direct('A', 'C', 35);
        graph.connect_direct('A', 'D', 40);

        graph.connect_direct('B', 'E', 25);
        graph.connect_direct('B', 'D', 20);

        graph.connect_direct('C', 'E', -30);
        graph.connect_direct('C', 'F', 30);

        graph.connect_direct('E', 'D', 45);
        graph.connect_direct('E', 'F', 25);

        graph.connect_direct('D', 'F', 20);

        print(graph);

        run_shortest_path(&bellman_ford_shortest_path, graph, 'A', 'F');
    }
}
