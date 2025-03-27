#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <algorithm>
#include "dungeon_manager.h"

bool isValidNumber(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

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
            if (n == 0 || n > 1000) {
                std::cerr << "Error: n must be between 1 and 1000." << std::endl;
                return false;
            }
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
            if (t1 == 0) {
                std::cerr << "Error: t1 must be greater than 0." << std::endl;
                return false;
            }
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
    uint32_t n = 0, t = 0, h = 0, d = 0;
    uint16_t t1 = 0, t2 = 0;

    if (!readConfig("config.txt", n, t, h, d, t1, t2)) {
        return 1;
    }

    if (n == 0) {
        std::cerr << "Error: Values must be greater than zero." << std::endl;
        return 1;
    }
    if (t1 > t2 || t2 > 15 || t1 == 0) {
        std::cerr << "Error: t1 and t2 must be within a valid range (1 ≤ t1 ≤ t2 ≤ 15)." << std::endl;
        return 1;
    }

    DungeonManager manager(n, t, h, d, t1, t2);

    manager.processQueue();

    return 0;
}

