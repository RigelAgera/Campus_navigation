// Algorithm.h - 图算法接口声明：连通分量、Dijkstra最短路径、MST、关键节点/边分析
#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <string>
#include <vector>
#include <utility>
#include "LGraph.h"
#include "LocationInfo.h"

// Return type for layered-graph shortest path with K coupons
struct KPathResult {
    int total_time;                                          // total walk_time on the path
    int k_used;                                              // actual coupons used
    std::vector<std::string> path;                           // place_id sequence
    std::vector<std::pair<std::string, std::string>> fast_edges; // edges where coupons were applied
    bool reachable;                                          // false if no path exists
};

class Algorithm {
public:
    // ==================== A. Connected Components ====================
    // Returns: (number_of_components, sizes_of_each_component)
    // Only traverses "open" edges. Isolated vertices count as size-1 components.
    static std::pair<int, std::vector<int>> connectedComponents(const LGraph& graph);

    // ==================== B. Shortest Path (Dijkstra) ====================
    // useDistance=true  → DIST mode  (weight = road.distance)
    // useDistance=false → TIME mode  (weight = road.walk_time)
    // Returns: (total_cost, path_of_place_ids from start to end)
    // If the target is unreachable, returns (-1, empty_vector)
    static std::pair<int, std::vector<std::string>> shortestPath(
        const LGraph& graph,
        const std::string& start_id,
        const std::string& end_id,
        bool useDistance
    );

    // ==================== B'. Time-Constrained Shortest Path ====================
    // Additional constraint: every place on the path must be open at `current_time`
    // (open_time <= current_time <= close_time, compare as strings "HH:MM")
    // If start or end itself is closed at that time → unreachable
    static std::pair<int, std::vector<std::string>> shortestPathWithTime(
        const LGraph& graph,
        const std::string& start_id,
        const std::string& end_id,
        bool useDistance,
        const std::string& current_time   // "HH:MM"
    );

    // ==================== C. Waypoint Path ====================
    // Go: start → waypoints[0] → waypoints[1] → ... → waypoints[n-1] → end
    // Each segment uses Dijkstra. If any segment fails → whole path fails.
    // waypoints must be visited in the given order.
    static std::pair<int, std::vector<std::string>> waypointPath(
        const LGraph& graph,
        const std::string& start_id,
        const std::vector<std::string>& waypoints,   // may be empty
        const std::string& end_id,
        bool useDistance
    );

    // ==================== X1. Layered-Graph Shortest Path (K coupons) ====================
    // At most K edges can be "accelerated": walk_time → ceil(walk_time / 3).
    // Only considers status == "open" edges. Weight used is always walk_time.
    static KPathResult shortestPathWithKCoupons(
        const LGraph& graph,
        const std::string& start_id,
        const std::string& end_id,
        int K
    );

    // ==================== D. Minimum Spanning Tree (Kruskal) ====================
    // Weight = distance. Only considers "open" edges.
    // Returns: (total_distance, vector_of_edges_in_MST)
    // If the open-edge graph is disconnected → total_distance = -1, edges = empty
    static std::pair<int, std::vector<RoadInfo>> mst(const LGraph& graph);

    // ==================== E. Critical Nodes & Edges ====================
    // Critical node: removing it (and all its incident edges) increases CC count
    // Critical edge: removing it (only that one undirected edge) increases CC count
    static std::vector<std::string> criticalNodes(const LGraph& graph);
    static std::vector<std::pair<std::string, std::string>> criticalEdges(const LGraph& graph);

private:
    // Helper: count connected components on current open-edge graph.
    // `skip_node`: if non-empty, treat this node as deleted (skip in BFS & adjacency).
    // `skip_edge`: if non-empty pair, treat this undirected edge as deleted.
    static int countComponents(
        const LGraph& graph,
        const std::string& skip_node,
        const std::pair<std::string, std::string>& skip_edge
    );
};

#endif