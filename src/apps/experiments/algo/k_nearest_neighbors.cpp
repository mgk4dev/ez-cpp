
#include <map>
#include <print>

#include <math.h>

#include <vector>

using namespace std;

struct Point {
    int x, y;
};

double distance(Point lhs, Point rhs) { return pow(lhs.x - rhs.x, 2) + pow(lhs.y - rhs.y, 2); }

vector<Point> k_nearest_neighbors(Point reference, const vector<Point>& points, uint k)
{
    multimap<double, Point> result;

    for (auto& point : points) {
        auto d = distance(reference, point);

        if (result.size() < k)
            result.insert({d, point});
        else if (result.rbegin()->first > d) {
            result.insert({d, point});
            result.erase(std::prev(result.end()));
        }
    }

    vector<Point> result_points;

    for (auto& [_, p] : result) result_points.push_back(p);

    return result_points;
}

int main()
{
    vector<Point> points{{1, 1}, {10, 10}, {20, 2}, {1, 1}, {3, 30}, {1, 1},
                         {4, 4}, {1, 1},   {25, 2}, {1, 1}, {3, 3},  {4, 4}};

    uint k = 3;

    for (auto p : k_nearest_neighbors({0, 0}, points, k)) { println("({},{})", p.x, p.y); }
}
