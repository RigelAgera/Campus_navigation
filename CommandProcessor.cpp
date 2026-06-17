// CommandProcessor.cpp - CLI command parsing and dispatch implementation
// All output formatting must strictly follow the command interface specification.

#include "CommandProcessor.h"
#include "Algorithm.h"
#include "CsvIO.h"
#include "GraphException.h"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>

namespace {
struct MenuCommand {
    int number;
    const char* name;
    const char* usage;
};

bool isValidMode(const std::string& mode) {
    return mode == "DIST" || mode == "TIME";
}

const MenuCommand kMenuCommands[] = {
    {1,  "LOAD",           "LOAD <places_file> <roads_file>"},
    {2,  "SAVE",           "SAVE <places_out_file> <roads_out_file>"},
    {3,  "QUERY_PLACE",    "QUERY_PLACE <place_id>"},
    {4,  "QUERY_CATEGORY", "QUERY_CATEGORY <category>"},
    {5,  "ADJ",            "ADJ <place_id>"},
    {6,  "ADD_PLACE",      "ADD_PLACE <place_id> <display_name> <category> <stay_time> <open_time> <close_time>"},
    {7,  "DELETE_PLACE",   "DELETE_PLACE <place_id>"},
    {8,  "UPDATE_PLACE",   "UPDATE_PLACE <place_id> <field> <value>"},
    {9,  "ADD_ROAD",       "ADD_ROAD <from_id> <to_id> <distance> <walk_time> <status>"},
    {10, "DELETE_ROAD",    "DELETE_ROAD <from_id> <to_id>"},
    {11, "UPDATE_ROAD",    "UPDATE_ROAD <from_id> <to_id> <field> <value>"},
    {12, "CLOSE_ROAD",     "CLOSE_ROAD <from_id> <to_id>"},
    {13, "OPEN_ROAD",      "OPEN_ROAD <from_id> <to_id>"},
    {14, "COMPONENTS",     "COMPONENTS"},
    {15, "SHORTEST",       "SHORTEST <from_id> <to_id> <DIST|TIME>"},
    {16, "TIMED_SHORTEST", "TIMED_SHORTEST <start_time> <from_id> <to_id>"},
    {17, "MUST_PASS",      "MUST_PASS <from_id> <to_id> <pass_id> <DIST|TIME>"},
    {18, "MST",            "MST"},
    {19, "CRITICAL",       "CRITICAL"},
    {20, "SHORTEST_K",     "SHORTEST_K <from_id> <to_id> <k>"},
    {21, "QUIT",           "QUIT"}
};
}

// ==================== Tokenisation ====================

std::vector<std::string> CommandProcessor::tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

void CommandProcessor::printMenu() {
    std::cout << "================ Campus Navigation Command Menu ================\n";
    for (const auto& command : kMenuCommands) {
        std::cout << command.number << ". " << command.usage << '\n';
    }
    std::cout << "---------------------------------------------------------------\n";
    std::cout << "Type a full command, or enter: number + arguments\n";
    std::cout << "Example: 1 saved_places.csv saved_roads.csv\n";
    std::cout << "cmd> ";
}

bool CommandProcessor::isNumberSelection(const std::string& line) {
    std::istringstream iss(line);
    std::string firstToken;
    if (!(iss >> firstToken)) {
        return false;
    }

    return std::all_of(firstToken.begin(), firstToken.end(),
                       [](unsigned char ch) { return std::isdigit(ch) != 0; });
}

std::string CommandProcessor::buildCommandFromSelection(const std::string& line) {
    std::istringstream iss(line);
    int selection = 0;
    iss >> selection;

    const MenuCommand* selectedCommand = nullptr;
    for (const auto& command : kMenuCommands) {
        if (command.number == selection) {
            selectedCommand = &command;
            break;
        }
    }

    if (selectedCommand == nullptr) {
        return std::string();
    }

    std::string remainingArgs;
    std::getline(iss, remainingArgs);
    if (!remainingArgs.empty() && remainingArgs.front() == ' ') {
        remainingArgs.erase(0, 1);
    }

    if (remainingArgs.empty()) {
        return selectedCommand->name;
    }
    return std::string(selectedCommand->name) + " " + remainingArgs;
}

// ==================== Main loop ====================

void CommandProcessor::run() {
    std::string line;
    printMenu();
    while (std::getline(std::cin, line)) {
        // Skip empty lines and lines containing only whitespace
        if (line.find_first_not_of(" \t\r") == std::string::npos) {
            std::cout << "cmd> ";
            continue;
        }

        if (isNumberSelection(line)) {
            std::string convertedCommand = buildCommandFromSelection(line);
            if (convertedCommand.empty()) {
                std::cout << "ERROR unknown_command" << std::endl;
                std::cout << "cmd> ";
                continue;
            }
            line = convertedCommand;
        }

        auto tokens = tokenize(line);
        if (tokens.empty()) {
            std::cout << "cmd> ";
            continue;
        }

        const std::string& cmd = tokens[0];

        try {
            if (cmd == "LOAD")                    handleLoad(tokens);
            else if (cmd == "SAVE")               handleSave(tokens);
            else if (cmd == "QUERY_PLACE")        handleQueryPlace(tokens);
            else if (cmd == "QUERY_CATEGORY")     handleQueryCategory(tokens);
            else if (cmd == "ADJ")                handleAdj(tokens);
            else if (cmd == "ADD_PLACE")          handleAddPlace(tokens);
            else if (cmd == "DELETE_PLACE")       handleDeletePlace(tokens);
            else if (cmd == "UPDATE_PLACE")       handleUpdatePlace(tokens);
            else if (cmd == "ADD_ROAD")           handleAddRoad(tokens);
            else if (cmd == "DELETE_ROAD")        handleDeleteRoad(tokens);
            else if (cmd == "UPDATE_ROAD")        handleUpdateRoad(tokens);
            else if (cmd == "CLOSE_ROAD")         handleCloseRoad(tokens);
            else if (cmd == "OPEN_ROAD")          handleOpenRoad(tokens);
            else if (cmd == "COMPONENTS")         handleComponents(tokens);
            else if (cmd == "SHORTEST")           handleShortest(tokens);
            else if (cmd == "TIMED_SHORTEST")     handleTimedShortest(tokens);
            else if (cmd == "MUST_PASS")          handleMustPass(tokens);
            else if (cmd == "MST")                handleMst(tokens);
            else if (cmd == "CRITICAL")           handleCritical(tokens);
            else if (cmd == "SHORTEST_K")         handleShortestK(tokens);
            else if (cmd == "QUIT")               return;   // no output, exit
            else {
                std::cout << "ERROR unknown_command" << std::endl;
            }
        } catch (const PlaceNotFoundException&) {
            std::cout << "ERROR place_not_found" << std::endl;
        } catch (const RoadNotFoundException&) {
            std::cout << "ERROR road_not_found" << std::endl;
        } catch (const InvalidFieldException&) {
            std::cout << "ERROR invalid_field" << std::endl;
        } catch (const InvalidRoadStatusException&) {
            std::cout << "ERROR invalid_field" << std::endl;
        } catch (const GraphException& e) {
            // Generic fallback – print the exception's what()
            // In production, we might want to map more specifically.
            std::cout << "ERROR " << e.what() << std::endl;
        }

        std::cout << "cmd> ";
    }
}

// ==================== 1. LOAD ====================

void CommandProcessor::handleLoad(const std::vector<std::string>& tokens) {
    // LOAD <places_file> <roads_file>
    if (tokens.size() < 3) {
        std::cout << "ERROR unknown_command" << std::endl;
        return;
    }
    const std::string& placesFile = tokens[1];
    const std::string& roadsFile  = tokens[2];

    auto places = CsvIO::loadPlaces(placesFile);
    auto roads = CsvIO::loadRoads(roadsFile);

    LGraph newGraph;
    for (const auto& p : places) {
        newGraph.addPlace(p);
    }

    for (const auto& r : roads) {
        newGraph.addRoad(r);
    }

    graph_ = std::move(newGraph);

    std::cout << "OK" << std::endl;
}

// ==================== 2. SAVE ====================

void CommandProcessor::handleSave(const std::vector<std::string>& tokens) {
    // SAVE <places_out_file> <roads_out_file>
    if (tokens.size() < 3) {
        std::cout << "ERROR unknown_command" << std::endl;
        return;
    }
    const std::string& placesOut = tokens[1];
    const std::string& roadsOut  = tokens[2];

    auto places = graph_.getAllPlaces();
    auto roads  = graph_.getAllRoads();

    CsvIO::savePlaces(placesOut, places);
    CsvIO::saveRoads(roadsOut, roads);

    std::cout << "OK" << std::endl;
}

// ==================== 3. QUERY_PLACE ====================

void CommandProcessor::handleQueryPlace(const std::vector<std::string>& tokens) {
    // QUERY_PLACE <place_id>
    if (tokens.size() < 2) {
        std::cout << "ERROR unknown_command" << std::endl;
        return;
    }
    const std::string& place_id = tokens[1];

    PlaceInfo p = graph_.getPlace(place_id);  // throws PlaceNotFoundException if missing
    std::cout << "PLACE " << p.place_id << " "
              << p.display_name << " "
              << p.category << " "
              << p.stay_time << " "
              << p.open_time << " "
              << p.close_time << std::endl;
}

// ==================== 4. QUERY_CATEGORY ====================

void CommandProcessor::handleQueryCategory(const std::vector<std::string>& tokens) {
    // QUERY_CATEGORY <category>
    if (tokens.size() < 2) {
        std::cout << "ERROR unknown_command" << std::endl;
        return;
    }
    const std::string& category = tokens[1];

    auto places = graph_.getPlacesByCategory(category);

    // Sort place_id in lexicographic ascending order
    std::sort(places.begin(), places.end(),
              [](const PlaceInfo& a, const PlaceInfo& b) {
                  return a.place_id < b.place_id;
              });

    std::cout << "CATEGORY " << category << " " << places.size();
    for (const auto& p : places) {
        std::cout << " " << p.place_id;
    }
    std::cout << std::endl;
}

// ==================== 5. ADJ ====================

void CommandProcessor::handleAdj(const std::vector<std::string>& tokens) {
    // ADJ <place_id>
    if (tokens.size() < 2) {
        std::cout << "ERROR unknown_command" << std::endl;
        return;
    }
    const std::string& place_id = tokens[1];

    // getPlace will throw PlaceNotFoundException if missing
    graph_.getPlace(place_id);

    auto roads = graph_.getAdjacentRoads(place_id);

    // Sort by neighbor place_id in lexicographic ascending order
    std::sort(roads.begin(), roads.end(),
              [](const RoadInfo& a, const RoadInfo& b) {
                  return a.to_id < b.to_id;
              });

    std::cout << "ADJ " << place_id << " " << roads.size();
    for (const auto& r : roads) {
        std::cout << " " << r.to_id << ":"
                  << r.distance << ":"
                  << r.walk_time << ":"
                  << r.status;
    }
    std::cout << std::endl;
}

// ==================== 6. ADD_PLACE ====================

void CommandProcessor::handleAddPlace(const std::vector<std::string>& tokens) {
    // ADD_PLACE <place_id> <display_name> <category> <stay_time> <open_time> <close_time>
    if (tokens.size() < 7) {
        std::cout << "ERROR unknown_command" << std::endl;
        return;
    }

    PlaceInfo p;
    p.place_id     = tokens[1];
    p.display_name = tokens[2];
    p.category     = tokens[3];
    p.stay_time    = std::stoi(tokens[4]);
    p.open_time    = tokens[5];
    p.close_time   = tokens[6];

    if (graph_.hasPlace(p.place_id)) {
        std::cout << "ERROR place_already_exists" << std::endl;
        return;
    }

    graph_.addPlace(p);
    std::cout << "OK" << std::endl;
}

// ==================== 7. DELETE_PLACE ====================

void CommandProcessor::handleDeletePlace(const std::vector<std::string>& tokens) {
    // DELETE_PLACE <place_id>
    if (tokens.size() < 2) {
        std::cout << "ERROR unknown_command" << std::endl;
        return;
    }
    // removePlace throws PlaceNotFoundException if missing
    graph_.removePlace(tokens[1]);
    std::cout << "OK" << std::endl;
}

// ==================== 8. UPDATE_PLACE ====================

void CommandProcessor::handleUpdatePlace(const std::vector<std::string>& tokens) {
    // UPDATE_PLACE <place_id> <field> <value>
    if (tokens.size() < 4) {
        std::cout << "ERROR unknown_command" << std::endl;
        return;
    }
    const std::string& place_id = tokens[1];
    const std::string& field    = tokens[2];
    const std::string& value    = tokens[3];

    PlaceInfo p = graph_.getPlace(place_id);  // throws PlaceNotFoundException

    if (field == "display_name") {
        p.display_name = value;
    } else if (field == "category") {
        p.category = value;
    } else if (field == "stay_time") {
        p.stay_time = std::stoi(value);
    } else if (field == "open_time") {
        p.open_time = value;
    } else if (field == "close_time") {
        p.close_time = value;
    } else {
        throw InvalidFieldException(field);
    }

    graph_.modifyPlace(place_id, p);
    std::cout << "OK" << std::endl;
}

// ==================== 9. ADD_ROAD ====================

void CommandProcessor::handleAddRoad(const std::vector<std::string>& tokens) {
    // ADD_ROAD <from_id> <to_id> <distance> <walk_time> <status>
    if (tokens.size() < 6) {
        std::cout << "ERROR unknown_command" << std::endl;
        return;
    }

    RoadInfo r;
    r.from_id   = tokens[1];
    r.to_id     = tokens[2];
    r.distance  = std::stoi(tokens[3]);
    r.walk_time = std::stoi(tokens[4]);
    r.status    = tokens[5];

    // Check place existence first
    if (!graph_.hasPlace(r.from_id) || !graph_.hasPlace(r.to_id)) {
        std::cout << "ERROR place_not_found" << std::endl;
        return;
    }

    // Check road existence
    if (graph_.hasRoad(r.from_id, r.to_id)) {
        std::cout << "ERROR road_already_exists" << std::endl;
        return;
    }

    graph_.addRoad(r);
    std::cout << "OK" << std::endl;
}

// ==================== 10. DELETE_ROAD ====================

void CommandProcessor::handleDeleteRoad(const std::vector<std::string>& tokens) {
    // DELETE_ROAD <from_id> <to_id>
    if (tokens.size() < 3) {
        std::cout << "ERROR unknown_command" << std::endl;
        return;
    }
    const std::string& from_id = tokens[1];
    const std::string& to_id   = tokens[2];

    // Check existence first to report road_not_found (not place_not_found)
    if (!graph_.hasRoad(from_id, to_id)) {
        std::cout << "ERROR road_not_found" << std::endl;
        return;
    }

    graph_.removeRoad(from_id, to_id);
    std::cout << "OK" << std::endl;
}

// ==================== 11. UPDATE_ROAD ====================

void CommandProcessor::handleUpdateRoad(const std::vector<std::string>& tokens) {
    // UPDATE_ROAD <from_id> <to_id> <field> <value>
    if (tokens.size() < 5) {
        std::cout << "ERROR unknown_command" << std::endl;
        return;
    }
    const std::string& from_id = tokens[1];
    const std::string& to_id   = tokens[2];
    const std::string& field   = tokens[3];
    const std::string& value   = tokens[4];

    // Check road existence
    if (!graph_.hasRoad(from_id, to_id)) {
        std::cout << "ERROR road_not_found" << std::endl;
        return;
    }

    // Get current road info (we need to construct it from the adjacency list)
    // We'll read existing values from one direction, modify the relevant field, then call modifyRoad
    auto adjRoads = graph_.getAdjacentRoads(from_id);
    RoadInfo r;
    bool found = false;
    for (const auto& road : adjRoads) {
        if (road.to_id == to_id) {
            r = road;
            found = true;
            break;
        }
    }
    // (should always be found since hasRoad returned true)

    if (field == "distance") {
        r.distance = std::stoi(value);
    } else if (field == "walk_time") {
        r.walk_time = std::stoi(value);
    } else if (field == "status") {
        r.status = value;
    } else {
        throw InvalidFieldException(field);
    }

    graph_.modifyRoad(from_id, to_id, r);
    std::cout << "OK" << std::endl;
}

// ==================== 12. CLOSE_ROAD ====================

void CommandProcessor::handleCloseRoad(const std::vector<std::string>& tokens) {
    // CLOSE_ROAD <from_id> <to_id>
    if (tokens.size() < 3) {
        std::cout << "ERROR unknown_command" << std::endl;
        return;
    }
    const std::string& from_id = tokens[1];
    const std::string& to_id   = tokens[2];

    if (!graph_.hasRoad(from_id, to_id)) {
        std::cout << "ERROR road_not_found" << std::endl;
        return;
    }

    graph_.setRoadStatus(from_id, to_id, "closed");
    std::cout << "OK" << std::endl;
}

// ==================== 13. OPEN_ROAD ====================

void CommandProcessor::handleOpenRoad(const std::vector<std::string>& tokens) {
    // OPEN_ROAD <from_id> <to_id>
    if (tokens.size() < 3) {
        std::cout << "ERROR unknown_command" << std::endl;
        return;
    }
    const std::string& from_id = tokens[1];
    const std::string& to_id   = tokens[2];

    if (!graph_.hasRoad(from_id, to_id)) {
        std::cout << "ERROR road_not_found" << std::endl;
        return;
    }

    graph_.setRoadStatus(from_id, to_id, "open");
    std::cout << "OK" << std::endl;
}

// ==================== 14. COMPONENTS ====================

void CommandProcessor::handleComponents(const std::vector<std::string>& tokens) {
    auto [count, sizes] = Algorithm::connectedComponents(graph_);

    // Sort sizes in descending order
    std::sort(sizes.begin(), sizes.end(), std::greater<int>());

    std::cout << "COMPONENTS " << count << " SIZES";
    for (int s : sizes) {
        std::cout << " " << s;
    }
    std::cout << std::endl;
}

// ==================== 15. SHORTEST ====================

void CommandProcessor::handleShortest(const std::vector<std::string>& tokens) {
    // SHORTEST <from_id> <to_id> <DIST|TIME>
    if (tokens.size() < 4) {
        std::cout << "ERROR unknown_command" << std::endl;
        return;
    }
    const std::string& from_id = tokens[1];
    const std::string& to_id   = tokens[2];
    const std::string& mode    = tokens[3];  // "DIST" or "TIME"

    if (!isValidMode(mode)) {
        std::cout << "ERROR invalid_field" << std::endl;
        return;
    }

    bool useDistance = (mode == "DIST");

    // Check place existence
    if (!graph_.hasPlace(from_id) || !graph_.hasPlace(to_id)) {
        std::cout << "ERROR place_not_found" << std::endl;
        return;
    }

    auto [total_cost, path] = Algorithm::shortestPath(graph_, from_id, to_id, useDistance);

    if (total_cost == -1) {
        std::cout << "NO_PATH" << std::endl;
        return;
    }

    std::cout << "PATH " << mode << " " << total_cost << " NODES";
    for (const auto& id : path) {
        std::cout << " " << id;
    }
    std::cout << std::endl;
}

// ==================== 15'. TIMED_SHORTEST ====================

void CommandProcessor::handleTimedShortest(const std::vector<std::string>& tokens) {
    // TIMED_SHORTEST <from_id> <to_id> <time> <DIST|TIME>
    if (tokens.size() < 5) {
        std::cout << "ERROR unknown_command" << std::endl;
        return;
    }
    const std::string& from_id      = tokens[1];
    const std::string& to_id        = tokens[2];
    const std::string& current_time = tokens[3];  // "HH:MM"
    const std::string& mode         = tokens[4];   // "DIST" or "TIME"

    if (!isValidMode(mode)) {
        std::cout << "ERROR invalid_field" << std::endl;
        return;
    }

    bool useDistance = (mode == "DIST");

    // Check place existence
    if (!graph_.hasPlace(from_id) || !graph_.hasPlace(to_id)) {
        std::cout << "ERROR place_not_found" << std::endl;
        return;
    }

    auto [total_cost, path] = Algorithm::shortestPathWithTime(
        graph_, from_id, to_id, useDistance, current_time);

    if (total_cost == -1) {
        std::cout << "NO_PATH" << std::endl;
        return;
    }

    std::cout << "PATH " << mode << " " << total_cost << " NODES";
    for (const auto& id : path) {
        std::cout << " " << id;
    }
    std::cout << std::endl;
}

// ==================== 16. MUST_PASS ====================

void CommandProcessor::handleMustPass(const std::vector<std::string>& tokens) {
    // MUST_PASS <from_id> <to_id> <DIST|TIME> <k> <p1> <p2> ... <pk>
    if (tokens.size() < 6) {
        std::cout << "ERROR unknown_command" << std::endl;
        return;
    }
    const std::string& from_id = tokens[1];
    const std::string& to_id   = tokens[2];
    const std::string& mode    = tokens[3];  // "DIST" or "TIME"
    int k = std::stoi(tokens[4]);

    if (!isValidMode(mode)) {
        std::cout << "ERROR invalid_field" << std::endl;
        return;
    }

    if (k < 0 || static_cast<int>(tokens.size()) != 5 + k) {
        std::cout << "ERROR unknown_command" << std::endl;
        return;
    }

    bool useDistance = (mode == "DIST");

    // Check place existence
    if (!graph_.hasPlace(from_id) || !graph_.hasPlace(to_id)) {
        std::cout << "ERROR place_not_found" << std::endl;
        return;
    }

    // Build waypoints vector
    std::vector<std::string> waypoints;
    for (int i = 0; i < k; ++i) {
        const std::string& waypoint = tokens[5 + i];
        if (!graph_.hasPlace(waypoint)) {
            std::cout << "ERROR place_not_found" << std::endl;
            return;
        }
        waypoints.push_back(waypoint);
    }

    auto [total_cost, path] = Algorithm::waypointPath(
        graph_, from_id, waypoints, to_id, useDistance);

    if (total_cost == -1) {
        std::cout << "NO_PATH" << std::endl;
        return;
    }

    std::cout << "PATH " << mode << " " << total_cost << " NODES";
    for (const auto& id : path) {
        std::cout << " " << id;
    }
    std::cout << std::endl;
}

// ==================== 17. MST ====================

void CommandProcessor::handleMst(const std::vector<std::string>& tokens) {
    auto [total_distance, edges] = Algorithm::mst(graph_);

    if (total_distance == -1) {
        std::cout << "DISCONNECTED" << std::endl;
        return;
    }

    // Sort edges by (min(u,v), max(u,v)) lexicographically
    std::sort(edges.begin(), edges.end(),
              [](const RoadInfo& a, const RoadInfo& b) {
                  // edges already have from_id < to_id from getAllOpenRoads
                  if (a.from_id != b.from_id) return a.from_id < b.from_id;
                  return a.to_id < b.to_id;
              });

    std::cout << "MST " << total_distance << " EDGES";
    for (const auto& e : edges) {
        std::cout << " " << e.from_id << "-" << e.to_id << ":" << e.distance;
    }
    std::cout << std::endl;
}

// ==================== 18. CRITICAL ====================

void CommandProcessor::handleCritical(const std::vector<std::string>& tokens) {
    auto nodes = Algorithm::criticalNodes(graph_);
    auto edges = Algorithm::criticalEdges(graph_);

    // Sort nodes by place_id lexicographically
    std::sort(nodes.begin(), nodes.end());

    // Sort edges by (min(u,v), max(u,v)) lexicographically
    std::sort(edges.begin(), edges.end(),
              [](const std::pair<std::string, std::string>& a,
                 const std::pair<std::string, std::string>& b) {
                  if (a.first != b.first) return a.first < b.first;
                  return a.second < b.second;
              });

    std::cout << "CRITICAL NODES " << nodes.size();
    for (const auto& n : nodes) {
        std::cout << " " << n;
    }
    std::cout << " EDGES " << edges.size();
    for (const auto& e : edges) {
        std::cout << " " << e.first << "-" << e.second;
    }
    std::cout << std::endl;
}

// ==================== X1. SHORTEST_K ====================

void CommandProcessor::handleShortestK(const std::vector<std::string>& tokens) {
    // SHORTEST_K <from_id> <to_id> <K>
    if (tokens.size() < 4) {
        std::cout << "ERROR unknown_command" << std::endl;
        return;
    }
    const std::string& from_id = tokens[1];
    const std::string& to_id   = tokens[2];
    int K = std::stoi(tokens[3]);

    // Check place existence
    if (!graph_.hasPlace(from_id) || !graph_.hasPlace(to_id)) {
        std::cout << "ERROR place_not_found" << std::endl;
        return;
    }

    auto result = Algorithm::shortestPathWithKCoupons(graph_, from_id, to_id, K);

    if (!result.reachable) {
        std::cout << "NO_PATH" << std::endl;
        return;
    }

    std::cout << "PATH " << result.total_time
              << " K_USED " << result.k_used
              << " NODES";
    for (const auto& id : result.path) {
        std::cout << " " << id;
    }
    std::cout << " FAST " << result.fast_edges.size();
    for (const auto& e : result.fast_edges) {
        std::cout << " " << e.first << "-" << e.second;
    }
    std::cout << std::endl;
}
