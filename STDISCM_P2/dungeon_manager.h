#pragma once
#include <iostream>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <cstdint>
#include <random>

class DungeonManager {
public:
    DungeonManager(uint32_t n, uint32_t t, uint32_t h, uint32_t d, uint16_t t1, uint16_t t2);
    void processQueue();

private:
    uint32_t maxInstances;
    uint16_t minTime, maxTime;
    uint32_t tanks;
    uint32_t healers;
    uint32_t dps;

    struct InstanceStatus {
        bool active;
        uint32_t partyCount;
        uint32_t totalTime;
    };
    std::vector<InstanceStatus> instanceStatus;

    std::mutex queueMutex;
    std::condition_variable instanceAvailable;
    std::queue<std::thread> instanceThreads;

    void runInstance(uint32_t slotIndex);
};
