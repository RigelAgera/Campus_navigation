// CommandProcessor.h - CLI command parsing and dispatch module
// Reads commands from stdin line by line, calls LGraph / Algorithm accordingly,
// and outputs formatted results to stdout per the command interface specification.

#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include <string>
#include <vector>
#include "LGraph.h"

class CommandProcessor {
public:
    explicit CommandProcessor(LGraph& graph) : graph_(graph) {}

    // Read lines from stdin until QUIT, dispatch each command.
    void run();

private:
    LGraph& graph_;

    // -------- Tokenisation --------
    // Split a line into whitespace-separated tokens.
    static std::vector<std::string> tokenize(const std::string& line);

    // -------- Command handlers (one per command) --------
    void handleLoad(const std::vector<std::string>& tokens);
    void handleSave(const std::vector<std::string>& tokens);
    void handleQueryPlace(const std::vector<std::string>& tokens);
    void handleQueryCategory(const std::vector<std::string>& tokens);
    void handleAdj(const std::vector<std::string>& tokens);
    void handleAddPlace(const std::vector<std::string>& tokens);
    void handleDeletePlace(const std::vector<std::string>& tokens);
    void handleUpdatePlace(const std::vector<std::string>& tokens);
    void handleAddRoad(const std::vector<std::string>& tokens);
    void handleDeleteRoad(const std::vector<std::string>& tokens);
    void handleUpdateRoad(const std::vector<std::string>& tokens);
    void handleCloseRoad(const std::vector<std::string>& tokens);
    void handleOpenRoad(const std::vector<std::string>& tokens);
    void handleComponents(const std::vector<std::string>& tokens);
    void handleShortest(const std::vector<std::string>& tokens);
    void handleTimedShortest(const std::vector<std::string>& tokens);
    void handleMustPass(const std::vector<std::string>& tokens);
    void handleMst(const std::vector<std::string>& tokens);
    void handleCritical(const std::vector<std::string>& tokens);
};

#endif