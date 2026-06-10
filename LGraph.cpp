// LGraph.cpp - 图 ADT 的实现

#include "LGraph.h"
#include "GraphException.h"
#include <algorithm>   // std::remove_if, std::minmax
#include <set>
#include <stdexcept>
#include <utility>     // std::pair

// ==================== 顶点操作 ====================

void LGraph::addPlace(const PlaceInfo& place) {
    // 如果已存在，可以覆盖或抛异常；这里选择覆盖（符合"修改"的语义）
    places_[place.place_id] = place;
    // 确保邻接表中也有这个顶点的条目（即使它还没有边）
    adjList_.emplace(place.place_id, std::vector<Edge>());
}

void LGraph::removePlace(const std::string& place_id) {
    auto placeIt = places_.find(place_id);
    if (placeIt == places_.end()) {
        throw PlaceNotFoundException(place_id);
    }

    // 从所有邻居的邻接表中移除指向本顶点的边
    for (const auto& edge : adjList_[place_id]) {
        auto& neighborEdges = adjList_[edge.to_id];
        neighborEdges.erase(
            std::remove_if(neighborEdges.begin(), neighborEdges.end(),
                [&](const Edge& e) { return e.to_id == place_id; }),
            neighborEdges.end()
        );
    }

    // 删除本顶点的邻接表和顶点信息
    adjList_.erase(place_id);
    places_.erase(place_id);
}

void LGraph::modifyPlace(const std::string& place_id, const PlaceInfo& new_info) {
    auto it = places_.find(place_id);
    if (it == places_.end()) {
        throw PlaceNotFoundException(place_id);
    }
    // 注意：place_id 本身不能改（它是键）
    // 但如果 new_info 里的 place_id 不同，可以选择忽略或抛异常
    it->second = new_info;
    // 确保 place_id 一致
    it->second.place_id = place_id;
}

bool LGraph::hasPlace(const std::string& place_id) const {
    return places_.find(place_id) != places_.end();
}

// ==================== 边操作 ====================

void LGraph::addRoad(const RoadInfo& road) {
    // 检查两个顶点是否存在
    if (!hasPlace(road.from_id)) {
        throw PlaceNotFoundException(road.from_id);
    }
    if (!hasPlace(road.to_id)) {
        throw PlaceNotFoundException(road.to_id);
    }

    if (hasRoad(road.from_id, road.to_id)) {
        throw DuplicateRoadException(road.from_id, road.to_id);
    }

    // 无向图：双向插入
    adjList_[road.from_id].push_back(
        {road.to_id, road.distance, road.walk_time, road.status});
    adjList_[road.to_id].push_back(
        {road.from_id, road.distance, road.walk_time, road.status});
}

void LGraph::removeRoad(const std::string& from_id, const std::string& to_id) {
    if (!hasRoad(from_id, to_id)) {
        throw RoadNotFoundException(from_id, to_id);
    }

    auto removeDirection = [&](const std::string& a, const std::string& b) {
        auto it = adjList_.find(a);
        if (it == adjList_.end()) return;
        auto& edges = it->second;
        edges.erase(
            std::remove_if(edges.begin(), edges.end(),
                [&](const Edge& e) { return e.to_id == b; }),
            edges.end()
        );
    };

    removeDirection(from_id, to_id);
    removeDirection(to_id, from_id);
}

void LGraph::modifyRoad(const std::string& from_id, const std::string& to_id,
                        const RoadInfo& new_info) {
    if (!hasRoad(from_id, to_id)) {
        throw RoadNotFoundException(from_id, to_id);
    }

    auto modifyDirection = [&](const std::string& a, const std::string& b) {
        auto it = adjList_.find(a);
        if (it == adjList_.end()) return;
        for (auto& e : it->second) {
            if (e.to_id == b) {
                e.distance  = new_info.distance;
                e.walk_time = new_info.walk_time;
                e.status    = new_info.status;
                return;
            }
        }
    };

    modifyDirection(from_id, to_id);
    modifyDirection(to_id, from_id);
}

bool LGraph::hasRoad(const std::string& from_id, const std::string& to_id) const {
    auto it = adjList_.find(from_id);
    if (it == adjList_.end()) return false;
    for (const auto& e : it->second) {
        if (e.to_id == to_id) return true;
    }
    return false;
}

void LGraph::setRoadStatus(const std::string& from_id, const std::string& to_id,
                           const std::string& status) {
    if (status != "open" && status != "closed") {
        throw InvalidRoadStatusException(status);
    }

    if (!hasRoad(from_id, to_id)) {
        throw RoadNotFoundException(from_id, to_id);
    }

    auto setDir = [&](const std::string& a, const std::string& b) {
        auto it = adjList_.find(a);
        if (it == adjList_.end()) return;
        for (auto& e : it->second) {
            if (e.to_id == b) {
                e.status = status;
                return;
            }
        }
    };

    setDir(from_id, to_id);
    setDir(to_id, from_id);
}

// ==================== 查询 ====================

PlaceInfo LGraph::getPlace(const std::string& place_id) const {
    auto it = places_.find(place_id);
    if (it == places_.end()) {
        throw PlaceNotFoundException(place_id);
    }
    return it->second;
}

std::vector<PlaceInfo> LGraph::getPlacesByCategory(const std::string& category) const {
    std::vector<PlaceInfo> result;
    for (const auto& [id, info] : places_) {
        if (info.category == category) {
            result.push_back(info);
        }
    }
    return result;
}

std::vector<RoadInfo> LGraph::getAdjacentRoads(const std::string& place_id) const {
    std::vector<RoadInfo> result;
    auto it = adjList_.find(place_id);
    if (it == adjList_.end()) {
        return result;  // 地点不存在或无边，返回空
    }

    for (const auto& edge : it->second) {
        RoadInfo r;
        r.from_id   = place_id;    // 以当前顶点为 from
        r.to_id     = edge.to_id;
        r.distance  = edge.distance;
        r.walk_time = edge.walk_time;
        r.status    = edge.status;
        result.push_back(std::move(r));
    }
    return result;
}

// ==================== 给保存模块 ====================

std::vector<PlaceInfo> LGraph::getAllPlaces() const {
    std::vector<PlaceInfo> result;
    result.reserve(places_.size());
    for (const auto& [id, info] : places_) {
        result.push_back(info);
    }
    std::sort(result.begin(), result.end(),
              [](const PlaceInfo& a, const PlaceInfo& b) {
                  return a.place_id < b.place_id;
              });
    return result;
}

std::vector<RoadInfo> LGraph::getAllRoads() const {
    std::vector<RoadInfo> result;
    std::set<std::pair<std::string, std::string>> visited;

    for (const auto& [from_id, edges] : adjList_) {
        for (const auto& edge : edges) {
            // 用 (较小ID, 较大ID) 作为唯一键去重
            auto key = std::minmax(from_id, edge.to_id);
            if (visited.insert(key).second) {
                RoadInfo r;
                r.from_id   = key.first;
                r.to_id     = key.second;
                r.distance  = edge.distance;
                r.walk_time = edge.walk_time;
                r.status    = edge.status;
                result.push_back(std::move(r));
            }
        }
    }

    std::sort(result.begin(), result.end(),
              [](const RoadInfo& a, const RoadInfo& b) {
                  if (a.from_id != b.from_id) return a.from_id < b.from_id;
                  return a.to_id < b.to_id;
              });
    return result;
}

// ==================== 给算法的接口 ====================

std::vector<std::pair<std::string, int>> LGraph::getOpenNeighbors(
    const std::string& place_id, bool useDistance) const
{
    std::vector<std::pair<std::string, int>> result;
    auto it = adjList_.find(place_id);
    if (it == adjList_.end()) return result;

    for (const auto& edge : it->second) {
        if (edge.status == "open") {
            int weight = useDistance ? edge.distance : edge.walk_time;
            result.emplace_back(edge.to_id, weight);
        }
    }
    return result;
}

std::vector<RoadInfo> LGraph::getAllOpenRoads() const {
    std::vector<RoadInfo> result;
    std::set<std::pair<std::string, std::string>> visited;

    for (const auto& [from_id, edges] : adjList_) {
        for (const auto& edge : edges) {
            if (edge.status != "open") continue;
            auto key = std::minmax(from_id, edge.to_id);
            if (visited.insert(key).second) {
                RoadInfo r;
                r.from_id   = key.first;
                r.to_id     = key.second;
                r.distance  = edge.distance;
                r.walk_time = edge.walk_time;
                r.status    = edge.status;
                result.push_back(std::move(r));
            }
        }
    }

    std::sort(result.begin(), result.end(),
              [](const RoadInfo& a, const RoadInfo& b) {
                  if (a.distance != b.distance) return a.distance < b.distance;
                  if (a.from_id != b.from_id) return a.from_id < b.from_id;
                  return a.to_id < b.to_id;
              });
    return result;
}

size_t LGraph::getPlaceCount() const {
    return places_.size();
}

size_t LGraph::getRoadCount() const {
    // 去重计数
    std::set<std::pair<std::string, std::string>> visited;
    for (const auto& [from_id, edges] : adjList_) {
        for (const auto& edge : edges) {
            visited.insert(std::minmax(from_id, edge.to_id));
        }
    }
    return visited.size();
}

void LGraph::clear() {
    places_.clear();
    adjList_.clear();
}
