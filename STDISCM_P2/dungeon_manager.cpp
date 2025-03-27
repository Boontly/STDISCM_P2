#include "dungeon_manager.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <algorithm>

DungeonManager::DungeonManager(uint32_t n, uint32_t t, uint32_t h, uint32_t d, uint16_t t1, uint16_t t2)
    : maxInstances(n), minTime(t1), maxTime(t2), tanks(t), healers(h), dps(d), nextInstanceIndex(0)
{
    instanceStatus.resize(maxInstances);
    for (uint32_t i = 0; i < maxInstances; ++i) {
        instanceStatus[i].active = false;
        instanceStatus[i].partyCount = 0;
        instanceStatus[i].totalTime = 0;
    }
}

void DungeonManager::processQueue() {
    uint32_t partiesFormed = 0;

    while (true) {
        std::unique_lock<std::mutex> lock(queueMutex);

        if (tanks < 1 || healers < 1 || dps < 3) {
            std::cout << "Not enough players left to form a party. Stopping queue processing." << std::endl;
            std::cout << "--------------------------------------" << std::endl;
            break;
        }

        int availableSlot = -1;
        for (uint32_t i = 0; i < maxInstances; i++) {
            uint32_t index = (nextInstanceIndex + i) % maxInstances;
            if (!instanceStatus[index].active) {
                availableSlot = static_cast<int>(index);
                nextInstanceIndex = (index + 1) % maxInstances;
                break;
            }
        }

        if (availableSlot == -1) {
            instanceAvailable.wait(lock, [this]() {
                return std::any_of(instanceStatus.begin(), instanceStatus.end(),
                    [](const InstanceStatus& inst) { return !inst.active; });
                });
            continue;
        }

        tanks--;
        healers--;
        dps -= 3;

        instanceStatus[availableSlot].active = true;
        instanceStatus[availableSlot].partyCount++;
        partiesFormed++;

        for (uint32_t i = 0; i < maxInstances; i++) {
            std::cout << "Instance " << i + 1 << " is "
                << (instanceStatus[i].active ? "active." : "empty.") << std::endl;
        }
        std::cout << "--------------------------------------" << std::endl;

        instanceThreads.emplace(std::thread(&DungeonManager::runInstance, this, static_cast<uint32_t>(availableSlot)));
        instanceThreads.back().detach();
    }

    while (true) {
        std::unique_lock<std::mutex> lock(queueMutex);
        bool anyActive = std::any_of(instanceStatus.begin(), instanceStatus.end(),
            [](const InstanceStatus& inst) { return inst.active; });
        if (!anyActive)
            break;
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "Summary of Instances:" << std::endl;
    for (uint32_t i = 0; i < maxInstances; i++) {
        std::cout << "Instance " << i + 1 << " had "
            << instanceStatus[i].partyCount << " parties for "
            << instanceStatus[i].totalTime << " seconds." << std::endl;
    }

    std::cout << "\nRemaining Players:" << std::endl;
    std::cout << "Tanks: " << tanks << "\nHealers: " << healers << "\nDPS: " << dps << std::endl;
}

void DungeonManager::runInstance(uint32_t slotIndex) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint16_t> dist(minTime, maxTime);
    uint16_t duration = dist(gen);

    std::this_thread::sleep_for(std::chrono::seconds(duration));

    {
        std::lock_guard<std::mutex> lock(queueMutex);
        instanceStatus[slotIndex].totalTime += duration;
        instanceStatus[slotIndex].active = false;
        for (uint32_t i = 0; i < maxInstances; i++) {
            std::cout << "Instance " << i + 1 << " is "
                << (instanceStatus[i].active ? "active." : "empty.") << std::endl;
        }
        std::cout << "--------------------------------------" << std::endl;
    }
    instanceAvailable.notify_one();
}
