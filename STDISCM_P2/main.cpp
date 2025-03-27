#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <algorithm>
#include "dungeon_manager.h"

// Function to check if a string is a valid non-negative number
bool isValidNumber(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

// Function to read config values from the file
bool readConfig(const std::string& filename, uint32_t& n, uint32_t& t, uint32_t& h, uint32_t& d, uint16_t& t1, uint16_t& t2) {
    std::ifstream configFile(filename);
    if (!configFile.is_open()) {
        std::cerr << "Error: Could not open " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        // Ignore empty lines
        if (line.empty()) continue;

        // Find key-value pairs
        if (line.find("n=") == 0) {
            std::string value = line.substr(2);
            if (!isValidNumber(value)) {
                std::cerr << "Error: Invalid value for n. Must be a positive integer." << std::endl;
                return false;
            }
            n = std::stoul(value);
        }
        else if (line.find("t=") == 0) {
            std::string value = line.substr(2);
            if (!isValidNumber(value)) {
                std::cerr << "Error: Invalid value for t. Must be a positive integer." << std::endl;
                return false;
            }
            t = std::stoul(value);
        }
        else if (line.find("h=") == 0) {
            std::string value = line.substr(2);
            if (!isValidNumber(value)) {
                std::cerr << "Error: Invalid value for h. Must be a positive integer." << std::endl;
                return false;
            }
            h = std::stoul(value);
        }
        else if (line.find("d=") == 0) {
            std::string value = line.substr(2);
            if (!isValidNumber(value)) {
                std::cerr << "Error: Invalid value for d. Must be a positive integer." << std::endl;
                return false;
            }
            d = std::stoul(value);
        }
        else if (line.find("t1=") == 0) {
            std::string value = line.substr(3);
            if (!isValidNumber(value)) {
                std::cerr << "Error: Invalid value for t1. Must be a positive integer." << std::endl;
                return false;
            }
            t1 = static_cast<uint16_t>(std::stoul(value));
        }
        else if (line.find("t2=") == 0) {
            std::string value = line.substr(3);
            if (!isValidNumber(value)) {
                std::cerr << "Error: Invalid value for t2. Must be a positive integer." << std::endl;
                return false;
            }
            t2 = static_cast<uint16_t>(std::stoul(value));
        }
        else {
            std::cerr << "Error: Unknown key in config file -> " << line << std::endl;
            return false;
        }
    }

    configFile.close();
    return true;
}

int main() {
    // Variables to store config values
    uint32_t n = 0, t = 0, h = 0, d = 0;
    uint16_t t1 = 0, t2 = 0;

    // Read configuration file
    if (!readConfig("config.txt", n, t, h, d, t1, t2)) {
        return 1; // Exit if config reading fails
    }

    // Validation
    if (n == 0 || t == 0 || h == 0 || d == 0) {
        std::cerr << "Error: Values must be greater than zero." << std::endl;
        return 1;
    }
    if (t1 > t2 || t2 > 15) {
        std::cerr << "Error: t1 and t2 must be within a valid range (t1 <= t2 <= 15)." << std::endl;
        return 1;
    }

    // Create DungeonManager instance
    DungeonManager manager(n, t, h, d, t1, t2);

    // Start dungeon processing
    manager.processQueue();

    return 0;
}
