// CsvIO.cpp - CSV 文件读写模块的实现
// test construction: # 批处理模式（评测用）./CampusNavigation < command.txt > answer.txt

// CsvIO.cpp - CSV 文件读写模块的实现
// 兼容带表头和不带表头两种形式，对空行做容错

#include "CsvIO.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cctype>

// ---------- 辅助函数（文件局部） ----------

// 去除字符串首尾空白字符
static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos)
        return "";
    return s.substr(start, end - start + 1);
}

// 将一行 CSV 文本按逗号拆分为字段列表（处理简单的引号包裹字段）
static std::vector<std::string> splitCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string cur;
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char ch = line[i];
        if (ch == '"') {
            inQuotes = !inQuotes;
        } else if (ch == ',' && !inQuotes) {
            fields.push_back(trim(cur));
            cur.clear();
        } else {
            cur += ch;
        }
    }
    fields.push_back(trim(cur));
    return fields;
}

// 判断一行是否为 places.csv 的表头
static bool isPlaceHeader(const std::string& line) {
    std::string lower = line;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower.find("place_id") != std::string::npos;
}

// 判断一行是否为 roads.csv 的表头
static bool isRoadHeader(const std::string& line) {
    std::string lower = line;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower.find("from_id") != std::string::npos;
}

// ---------- 公开接口 ----------

std::vector<PlaceInfo> CsvIO::loadPlaces(const std::string& filename) {
    std::vector<PlaceInfo> places;
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + filename);
    }

    std::string line;
    bool firstLine = true;

    while (std::getline(file, line)) {
        // 跳过空行
        if (line.empty() || line.find_first_not_of(" \t\r") == std::string::npos)
            continue;

        // 如果是第一行，检查是否为表头
        if (firstLine) {
            firstLine = false;
            if (isPlaceHeader(line))
                continue;   // 跳过表头
        }

        std::vector<std::string> fields = splitCsvLine(line);
        if (fields.size() < 6) {
            // 字段不足，跳过该行（容错处理）
            continue;
        }

        PlaceInfo place;
        place.place_id     = fields[0];
        place.display_name = fields[1];
        place.category     = fields[2];
        place.stay_time    = std::stoi(fields[3]);
        place.open_time    = fields[4];
        place.close_time   = fields[5];

        places.push_back(std::move(place));
    }

    return places;
}

std::vector<RoadInfo> CsvIO::loadRoads(const std::string& filename) {
    std::vector<RoadInfo> roads;
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + filename);
    }

    std::string line;
    bool firstLine = true;

    while (std::getline(file, line)) {
        // 跳过空行
        if (line.empty() || line.find_first_not_of(" \t\r") == std::string::npos)
            continue;

        // 如果是第一行，检查是否为表头
        if (firstLine) {
            firstLine = false;
            if (isRoadHeader(line))
                continue;   // 跳过表头
        }

        std::vector<std::string> fields = splitCsvLine(line);
        if (fields.size() < 5) {
            continue;
        }

        RoadInfo road;
        road.from_id   = fields[0];
        road.to_id     = fields[1];
        road.distance  = std::stoi(fields[2]);
        road.walk_time = std::stoi(fields[3]);
        road.status    = fields[4];

        roads.push_back(std::move(road));
    }

    return roads;
}

void CsvIO::savePlaces(const std::string& filename,
                       const std::vector<PlaceInfo>& places) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("无法写入文件: " + filename);
    }

    // 写出表头
    file << "place_id,display_name,category,stay_time,open_time,close_time\n";

    for (const auto& p : places) {
        file << p.place_id << ","
             << p.display_name << ","
             << p.category << ","
             << p.stay_time << ","
             << p.open_time << ","
             << p.close_time << "\n";
    }
}

void CsvIO::saveRoads(const std::string& filename,
                      const std::vector<RoadInfo>& roads) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("无法写入文件: " + filename);
    }

    // 写出表头
    file << "from_id,to_id,distancce,walk_time,status\n";

    for (const auto& r : roads) {
        file << r.from_id << ","
             << r.to_id << ","
             << r.distance << ","
             << r.walk_time << ","
             << r.status << "\n";
    }
}