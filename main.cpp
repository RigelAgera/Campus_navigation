// main.cpp - Program entry point
// Reads CLI commands from stdin in batch mode, outputs results to stdout.
// Usage: ./CampusNavigation < command.txt > answer.txt

#include "CommandProcessor.h"
#include "LGraph.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    // Ensure UTF-8 output/input in Windows console so Chinese text is displayed correctly.
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    LGraph graph;
    CommandProcessor processor(graph);
    processor.run();
    return 0;
}