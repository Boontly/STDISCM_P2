#include "dungeon_manager.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <algorithm>

DungeonManager::DungeonManager(uint32_t n, uint32_t t, uint32_t h, uint32_t d, uint16_t t1, uint16_t t2)
    : maxInstances(n), minTime(t1), maxTime(t2), tanks(t), healers(h), dps(d)
{
    // Initialize instance statuses: all empty with 0 parties and 0 total time.
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

        // Check if enough players exist to form a party
        if (tanks < 1 || healers < 1 || dps < 3) {
            std::cout << "Not enough players to form a party. Stopping queue processing." << std::endl;
            break;
        }

        // Find an available instance slot (one that's not active)
        int availableSlot = -1;
        for (uint32_t i = 0; i < maxInstances; i++) {
            if (!instanceStatus[i].active) {
                availableSlot = static_cast<int>(i);
                break;
            }
        }

        // If no slot is available, wait until one is free
        if (availableSlot == -1) {
            instanceAvailable.wait(lock, [this]() {
                return std::any_of(instanceStatus.begin(), instanceStatus.end(),
                    [](const InstanceStatus& inst) { return !inst.active; });
                });
            continue; // Re-check conditions after wake-up
        }

        // Deduct players for a full party
        tanks--;
        healers--;
        dps -= 3;

        // Mark the chosen instance as active and update its counters
        instanceStatus[availableSlot].active = true;
        instanceStatus[availableSlot].partyCount++;
        partiesFormed++;

        // Print the current status of all instance slots
        std::cout << "\nCurrent Instance Status:" << std::endl;
        for (uint32_t i = 0; i < maxInstances; i++) {
            std::cout << "Instance " << i + 1 << ": "
                << (instanceStatus[i].active ? "active" : "empty") << std::endl;
        }
        std::cout << std::endl;

        // Launch the dungeon instance thread for the available slot
        instanceThreads.emplace(std::thread(&DungeonManager::runInstance, this, static_cast<uint32_t>(availableSlot)));
        instanceThreads.back().detach();
    }

    // Wait for all running instances to complete
    while (true) {
        std::unique_lock<std::mutex> lock(queueMutex);
        bool anyActive = std::any_of(instanceStatus.begin(), instanceStatus.end(),
            [](const InstanceStatus& inst) { return inst.active; });
        if (!anyActive)
            break;
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Print final summary for each instance
    std::cout << "\nSummary of Instances:" << std::endl;
    for (uint32_t i = 0; i < maxInstances; i++) {
        std::cout << "Instance " << i + 1 << " served "
            << instanceStatus[i].partyCount << " parties and total time served "
            << instanceStatus[i].totalTime << " seconds." << std::endl;
    }
}

void DungeonManager::runInstance(uint32_t slotIndex) {
    // Create a random duration between minTime and maxTime
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint16_t> dist(minTime, maxTime);
    uint16_t duration = dist(gen);

    {
        // Protect output with a lock to avoid interleaving messages
        std::lock_guard<std::mutex> lock(queueMutex);
        std::cout << "Instance " << slotIndex + 1 << " active for " << duration << " seconds." << std::endl;
    }

    // Simulate dungeon run
    std::this_thread::sleep_for(std::chrono::seconds(duration));

    {
        // Update instance status: add to total time and mark as empty
        std::lock_guard<std::mutex> lock(queueMutex);
        instanceStatus[slotIndex].totalTime += duration;
        instanceStatus[slotIndex].active = false;
        std::cout << "Instance " << slotIndex + 1 << " completed." << std::endl;
    }
    instanceAvailable.notify_one();
}
