// main.cpp - Program entry point
// Reads CLI commands from stdin in batch mode, outputs results to stdout.
// Usage: ./CampusNavigation < command.txt > answer.txt

#include "CommandProcessor.h"
#include "LGraph.h"
#include <iostream>

int main() {
    LGraph graph;
    CommandProcessor processor(graph);
    processor.run();
    return 0;
}