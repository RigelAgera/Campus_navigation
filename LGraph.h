// LGraph.h - 图 ADT 公共接口声明
// 使用邻接表存储，顶点用 unordered_map 做 O(1) 索引

#ifndef LGRAPH_H
#define LGRAPH_H

#include <string>
#include <vector>
#include <unordered_map>
#include "LocationInfo.h"

class LGraph {
public:
    // ==================== 顶点操作 ====================
    void addPlace(const PlaceInfo& place);
    void removePlace(const std::string& place_id);
    void modifyPlace(const std::string& place_id, const PlaceInfo& new_info);
    bool hasPlace(const std::string& place_id) const;

    // ==================== 边操作 ====================
    void addRoad(const RoadInfo& road);
    void removeRoad(const std::string& from_id, const std::string& to_id);
    void modifyRoad(const std::string& from_id, const std::string& to_id,
                    const RoadInfo& new_info);
    void setRoadStatus(const std::string& from_id, const std::string& to_id,
                       const std::string& status);  // "open" / "closed"
    bool hasRoad(const std::string& from_id, const std::string& to_id) const;

    // ==================== 查询 ====================
    PlaceInfo getPlace(const std::string& place_id) const;
    std::vector<PlaceInfo> getPlacesByCategory(const std::string& category) const;
    // 返回某顶点的所有邻接道路（含 closed）
    std::vector<RoadInfo> getAdjacentRoads(const std::string& place_id) const;

    // ==================== 给保存模块和数据 ====================
    std::vector<PlaceInfo> getAllPlaces() const;
    std::vector<RoadInfo> getAllRoads() const;   // 已去重

    // ==================== 给算法的接口 ====================
    // 仅返回 status=="open" 的邻居，权重按 useDistance 选 distance 或 walk_time
    std::vector<std::pair<std::string, int>> getOpenNeighbors(
        const std::string& place_id, bool useDistance) const;
    // 返回所有 open 边（去重），供 Kruskal 使用
    std::vector<RoadInfo> getAllOpenRoads() const;

    size_t getPlaceCount() const;
    size_t getRoadCount() const;   // 返回去重后的道路数（open + closed）

    void clear();   // 清空所有顶点和边，供 LOAD 使用

private:
    // 邻接表中的一条边记录
    struct Edge {
        std::string to_id;  // store as directed graph
        int distance;
        int walk_time;
        std::string status;   // "open" 或 "closed"
    };

    // 核心存储
    std::unordered_map<std::string, PlaceInfo> places_;            // place_id → 地点信息
    std::unordered_map<std::string, std::vector<Edge>> adjList_;   // place_id → 邻接边列表
};

#endif