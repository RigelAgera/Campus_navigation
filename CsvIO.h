// CsvIO.h - CSV 文件读写模块接口声明，用于加载/保存 places.csv 和 roads.csv

#ifndef CSVIO_H
#define CSVIO_H

#include <string>
#include <vector>
#include "LocationInfo.h"

class CsvIO {
public:
    // 读取 places.csv，返回所有地点信息
    static std::vector<PlaceInfo> loadPlaces(const std::string& filename);
    
    // 读取 roads.csv，返回所有道路信息
    static std::vector<RoadInfo> loadRoads(const std::string& filename);
    
    // 将地点数据写入 places.csv
    static void savePlaces(const std::string& filename, 
                           const std::vector<PlaceInfo>& places);
    
    // 将道路数据写入 roads.csv
    static void saveRoads(const std::string& filename,
                          const std::vector<RoadInfo>& roads);
};

#endif