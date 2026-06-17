// Algorithm.cpp - 图算法的实现（连通分量、Dijkstra、MST、关键节点/边等）
#include "Algorithm.h"
#include "GraphException.h"
#include <queue>
#include <algorithm>
#include <limits>
#include <set>
#include <unordered_map>
#include <unordered_set>

using namespace std;

// ==================== Internal helper: DSU (Disjoint Set Union) for Kruskal ====================

class DSU {
    unordered_map<string, string> parent;
    unordered_map<string, int> rank;
public:
    void makeSet(const string& id) {
        parent[id] = id;
        rank[id] = 0;
    }

    string find(const string& x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);  // path compression
        }
        return parent[x];
    }

    void unite(const string& a, const string& b) {
        string ra = find(a), rb = find(b);
        if (ra == rb) return;
        if (rank[ra] < rank[rb]) {
            parent[ra] = rb;
        } else if (rank[ra] > rank[rb]) {
            parent[rb] = ra;
        } else {
            parent[rb] = ra;
            rank[ra]++;
        }
    }
};

// ==================== private helper: countComponents ====================

int Algorithm::countComponents(
    const LGraph& graph,
    const string& skip_node,
    const pair<string, string>& skip_edge)
{
    auto all_places = graph.getAllPlaces();
    unordered_set<string> visited;
    int comp_count = 0;

    for (const auto& place : all_places) {
        const string& id = place.place_id;
        if (id == skip_node) continue;           // treat as "deleted"
        if (visited.count(id)) continue;

        // BFS from this unvisited node
        comp_count++;
        queue<string> q;
        q.push(id);
        visited.insert(id);

        while (!q.empty()) {
            string cur = q.front(); q.pop();
            // Get open neighbors (weight arg doesn't matter for BFS,
            // always passing true to get distance weight — not used)
            auto neighbors = graph.getOpenNeighbors(cur, true);
            for (const auto& [neighbor_id, weight] : neighbors) {
                if (neighbor_id == skip_node) continue;  // treat as "deleted"

                // check if this specific edge is "deleted"
                // edges always stored as (min_id, max_id) from getAllOpenRoads
                pair<string, string> edge_key = {min(cur, neighbor_id),
                                                  max(cur, neighbor_id)};
                if (edge_key == skip_edge) continue;

                if (!visited.count(neighbor_id)) {
                    visited.insert(neighbor_id);
                    q.push(neighbor_id);
                }
            }
        }
    }
    return comp_count;
}

// ==================== A. Connected Components ====================

pair<int, vector<int>> Algorithm::connectedComponents(const LGraph& graph)
{
    auto all_places = graph.getAllPlaces();
    unordered_set<string> visited;
    vector<int> sizes;

    for (const auto& place : all_places) {
        const string& id = place.place_id;
        if (visited.count(id)) continue;

        int size = 0;
        queue<string> q;
        q.push(id);
        visited.insert(id);

        while (!q.empty()) {
            string cur = q.front(); q.pop();
            size++;

            auto neighbors = graph.getOpenNeighbors(cur, true);
            for (const auto& [neighbor_id, weight] : neighbors) {
                if (!visited.count(neighbor_id)) {
                    visited.insert(neighbor_id);
                    q.push(neighbor_id);
                }
            }
        }
        sizes.push_back(size);
    }
    return {static_cast<int>(sizes.size()), sizes};
}

// ==================== B. Shortest Path (Dijkstra) ====================

pair<int, vector<string>> Algorithm::shortestPath(
    const LGraph& graph,
    const string& start_id,
    const string& end_id,
    bool useDistance)
{
    // Validate existence of both endpoints
    if (!graph.hasPlace(start_id) || !graph.hasPlace(end_id)) {
        return {-1, {}};
    }

    unordered_map<string, int> dist;
    unordered_map<string, string> prev;

    auto all_places = graph.getAllPlaces();
    for (const auto& p : all_places) {
        dist[p.place_id] = numeric_limits<int>::max();
    }

    // Min-heap: (distance, place_id)
    using State = pair<int, string>;
    priority_queue<State, vector<State>, greater<State>> pq;

    dist[start_id] = 0;
    pq.push({0, start_id});

    while (!pq.empty()) {
        auto [cur_dist, cur_id] = pq.top(); pq.pop();

        // Lazy deletion: stale entry (a shorter path was already found)
        if (cur_dist != dist[cur_id]) continue;

        // Early exit once we've popped the target
        if (cur_id == end_id) break;

        auto neighbors = graph.getOpenNeighbors(cur_id, useDistance);
        for (const auto& [neighbor_id, weight] : neighbors) {
            int new_dist = cur_dist + weight;
            if (new_dist < dist[neighbor_id]) {
                dist[neighbor_id] = new_dist;
                prev[neighbor_id] = cur_id;
                pq.push({new_dist, neighbor_id});
            }
        }
    }

    // Unreachable
    if (dist[end_id] == numeric_limits<int>::max()) {
        return {-1, {}};
    }

    // Reconstruct path: walk backwards from end to start
    vector<string> path;
    for (string cur = end_id; cur != start_id; cur = prev[cur]) {
        path.push_back(cur);
    }
    path.push_back(start_id);
    reverse(path.begin(), path.end());

    return {dist[end_id], path};
}

// ==================== B'. Time-Constrained Shortest Path ====================

pair<int, vector<string>> Algorithm::shortestPathWithTime(
    const LGraph& graph,
    const string& start_id,
    const string& end_id,
    bool useDistance,
    const string& current_time)
{
    // Validate existence of both endpoints
    if (!graph.hasPlace(start_id) || !graph.hasPlace(end_id)) {
        return {-1, {}};
    }

    // Build set of place IDs that are CLOSED at current_time
    // "HH:MM" strings compare lexicographically (e.g., "08:00" < "12:30" fits design)
    unordered_set<string> closed_places;
    auto all_places = graph.getAllPlaces();
    for (const auto& p : all_places) {
        if (current_time < p.open_time || current_time > p.close_time) {
            closed_places.insert(p.place_id);
        }
    }

    // If start or end is closed → unreachable
    if (closed_places.count(start_id) || closed_places.count(end_id)) {
        return {-1, {}};
    }

    // Dijkstra, skipping neighbours that are closed at this time
    unordered_map<string, int> dist;
    unordered_map<string, string> prev;

    for (const auto& p : all_places) {
        dist[p.place_id] = numeric_limits<int>::max();
    }

    using State = pair<int, string>;
    priority_queue<State, vector<State>, greater<State>> pq;

    dist[start_id] = 0;
    pq.push({0, start_id});

    while (!pq.empty()) {
        auto [cur_dist, cur_id] = pq.top(); pq.pop();
        if (cur_dist != dist[cur_id]) continue;
        if (cur_id == end_id) break;

        auto neighbors = graph.getOpenNeighbors(cur_id, useDistance);
        for (const auto& [neighbor_id, weight] : neighbors) {
            if (closed_places.count(neighbor_id)) continue;  // *** time constraint ***
            int new_dist = cur_dist + weight;
            if (new_dist < dist[neighbor_id]) {
                dist[neighbor_id] = new_dist;
                prev[neighbor_id] = cur_id;
                pq.push({new_dist, neighbor_id});
            }
        }
    }

    if (dist[end_id] == numeric_limits<int>::max()) {
        return {-1, {}};
    }

    vector<string> path;
    for (string cur = end_id; cur != start_id; cur = prev[cur]) {
        path.push_back(cur);
    }
    path.push_back(start_id);
    reverse(path.begin(), path.end());

    return {dist[end_id], path};
}

// ==================== C. Waypoint Path ====================

pair<int, vector<string>> Algorithm::waypointPath(
    const LGraph& graph,
    const string& start_id,
    const vector<string>& waypoints,
    const string& end_id,
    bool useDistance)
{
    // Build the full sequence: start → w[0] → w[1] → ... → w[n-1] → end
    vector<string> sequence;
    sequence.push_back(start_id);
    for (const auto& wp : waypoints) {
        sequence.push_back(wp);
    }
    sequence.push_back(end_id);

    vector<string> full_path;
    int total_cost = 0;

    for (size_t i = 0; i + 1 < sequence.size(); i++) {
        auto [cost, path] = shortestPath(graph, sequence[i], sequence[i + 1], useDistance);
        if (cost == -1) {
            return {-1, {}};  // any segment fails → whole path fails
        }
        total_cost += cost;

        // Append to full_path.
        // If this is the first segment, take the whole path.
        // Otherwise, skip path[0] (it's already the last element of full_path).
        if (full_path.empty()) {
            full_path = path;
        } else {
            full_path.insert(full_path.end(), path.begin() + 1, path.end());
        }
    }

    return {total_cost, full_path};
}

// ==================== X1. Layered-Graph Shortest Path (K coupons) ====================

KPathResult Algorithm::shortestPathWithKCoupons(
    const LGraph& graph,
    const string& start_id,
    const string& end_id,
    int K)
{
    if (!graph.hasPlace(start_id) || !graph.hasPlace(end_id)) {
        return {-1, 0, {}, {}, false};
    }

    // dist[layer][place_id] = shortest time to reach this state
    // We use vector<unordered_map<string, int>> indexed by layer [0..K]
    auto all_places = graph.getAllPlaces();
    vector<unordered_map<string, int>> dist(K + 1);
    const int INF = numeric_limits<int>::max();
    for (int k = 0; k <= K; ++k) {
        for (const auto& p : all_places) {
            dist[k][p.place_id] = INF;
        }
    }

    // Previous-state for path reconstruction
    // prev[layer][place_id] = {previous_layer, previous_place_id, coupon_used_on_this_edge}
    struct PrevState {
        int prev_layer;
        string prev_place;
        bool coupon_used;   // true if this step used a coupon
    };
    vector<unordered_map<string, PrevState>> prev(K + 1);

    // Min-heap: (distance, layer, place_id)
    using State = tuple<int, int, string>;  // dist, layer, place_id
    priority_queue<State, vector<State>, greater<State>> pq;

    dist[0][start_id] = 0;
    pq.push({0, 0, start_id});

    while (!pq.empty()) {
        auto [cur_dist, cur_layer, cur_id] = pq.top(); pq.pop();

        // Lazy deletion
        if (cur_dist != dist[cur_layer][cur_id]) continue;

        // Early exit: once we pop any layer of end_id, we have the optimal answer
        // because Dijkstra guarantees first pop of any end state is optimal
        if (cur_id == end_id) {
            // Reconstruct path and fast edges
            KPathResult result;
            result.total_time = cur_dist;
            result.k_used = cur_layer;
            result.reachable = true;

            // Backtrack from (cur_layer, end_id) to (0, start_id)
            int layer = cur_layer;
            string node = end_id;
            while (!(layer == 0 && node == start_id)) {
                result.path.push_back(node);
                const auto& ps = prev[layer][node];
                if (ps.coupon_used) {
                    string u = min(ps.prev_place, node);
                    string v = max(ps.prev_place, node);
                    result.fast_edges.emplace_back(u, v);
                }
                layer = ps.prev_layer;
                node = ps.prev_place;
            }
            result.path.push_back(start_id);
            reverse(result.path.begin(), result.path.end());

            // Sort fast_edges by (min(u,v), max(u,v)) lexicographically
            sort(result.fast_edges.begin(), result.fast_edges.end(),
                 [](const pair<string, string>& a, const pair<string, string>& b) {
                     if (a.first != b.first) return a.first < b.first;
                     return a.second < b.second;
                 });
            return result;
        }

        auto neighbors = graph.getOpenNeighbors(cur_id, false);  // false → walk_time
        for (const auto& [neighbor_id, walk_time] : neighbors) {
            // Option A: don't use a coupon — stay in same layer
            {
                int new_dist = cur_dist + walk_time;
                if (new_dist < dist[cur_layer][neighbor_id]) {
                    dist[cur_layer][neighbor_id] = new_dist;
                    prev[cur_layer][neighbor_id] = {cur_layer, cur_id, false};
                    pq.push({new_dist, cur_layer, neighbor_id});
                }
            }

            // Option B: use a coupon — move to layer+1
            if (cur_layer < K) {
                int fast_time = (walk_time + 2) / 3;  // ceil(walk_time / 3)
                int new_dist = cur_dist + fast_time;
                if (new_dist < dist[cur_layer + 1][neighbor_id]) {
                    dist[cur_layer + 1][neighbor_id] = new_dist;
                    prev[cur_layer + 1][neighbor_id] = {cur_layer, cur_id, true};
                    pq.push({new_dist, cur_layer + 1, neighbor_id});
                }
            }
        }
    }

    // No path found — check all layers of end_id
    return {-1, 0, {}, {}, false};
}

// ==================== D. Minimum Spanning Tree (Kruskal) ====================

pair<int, vector<RoadInfo>> Algorithm::mst(const LGraph& graph)
{
    // Edge case: 0 or 1 places → MST is empty with cost 0
    if (graph.getPlaceCount() <= 1) {
        return {0, {}};
    }

    // Get all open roads, sorted deterministically by distance and endpoints
    vector<RoadInfo> roads = graph.getAllOpenRoads();

    // Initialise DSU with all places
    DSU dsu;
    auto all_places = graph.getAllPlaces();
    for (const auto& p : all_places) {
        dsu.makeSet(p.place_id);
    }

    vector<RoadInfo> mst_edges;
    int total_distance = 0;

    for (const auto& road : roads) {
        if (dsu.find(road.from_id) != dsu.find(road.to_id)) {
            dsu.unite(road.from_id, road.to_id);
            mst_edges.push_back(road);
            total_distance += road.distance;

            // Early stop: MST of V vertices has exactly V-1 edges
            if (mst_edges.size() == graph.getPlaceCount() - 1) {
                break;
            }
        }
    }

    // If we didn't collect V-1 edges, the open-edge graph is disconnected
    if (static_cast<size_t>(mst_edges.size()) != graph.getPlaceCount() - 1) {
        return {-1, {}};
    }

    return {total_distance, mst_edges};
}

// ==================== E. Critical Nodes & Edges ====================

vector<string> Algorithm::criticalNodes(const LGraph& graph)
{
    // Baseline: connected component count with nothing removed
    int baseline = countComponents(graph, "", {"", ""});

    vector<string> result;
    auto all_places = graph.getAllPlaces();     // vector<PlaceInfo>

    for (const auto& p : all_places) {
        int after_removal = countComponents(graph, p.place_id, {"", ""});
        if (after_removal > baseline) {
            result.push_back(p.place_id);
        }
    }
    return result;
}

vector<pair<string, string>> Algorithm::criticalEdges(const LGraph& graph)
{
    int baseline = countComponents(graph, "", {"", ""});

    vector<pair<string, string>> result;
    auto roads = graph.getAllOpenRoads();

    for (const auto& road : roads) {
        // Edges from getAllOpenRoads are stored with from_id < to_id (via minmax)
        pair<string, string> edge_key = {road.from_id, road.to_id};
        int after_removal = countComponents(graph, "", edge_key);
        if (after_removal > baseline) {
            result.push_back(edge_key);
        }
    }
    return result;
}