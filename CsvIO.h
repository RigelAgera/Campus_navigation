// CsvIO.h - CSV 文件读写模块接口声明，用于加载/保存 places.csv 和 roads.csv
// places.csv文件格式
/*
place_id,display_name,category,stay_time,open_time,close_time
P0001,Library,Teaching,30,08:00,22:00
P0002,Canteen,Dining,40,06:30,20:30
*/
// roads.csv文件格式
/*
from_id,to_id,distance,walk_time,status
P0001,P0002,180,3,open
P0001,P0003,240,4,closed
*/
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