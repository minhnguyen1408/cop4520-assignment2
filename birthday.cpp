//
// Created by Henry on 2/20/2024.
//
// This predicament resembles the N prisoners problem, and the prescribed
// algorithm necessitates the selection of a guest as the designated leader.
// The leader plays a pivotal role in effectively enumerating the number of guests
// by incrementing a global counter each time they revisit the party room with the
// cupcake. Analogous to the light in the prisoner problem, the cupcake serves as an
// indicator of whether a new guest has consumed a cupcake during the leader's absence.
// Guests decide whether to take or leave the cupcake based on whether they have already
// partaken in one. Importantly, only the leader retains the authority to replace a cupcake,
// necessitating a request to the servants for this task.
#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#define NUM_GUESTS 50

// Keeps track of which guests in the labyrinth have eaten the cupcake
std::array<bool, NUM_GUESTS> guestsPicked;
std::mutex mutex;
bool isCupcakeAvailable = true;
int currentCount = 0;
// Keeps track of which guest is currently in the labyrinth
unsigned int activeThreadIndex;
// Note: the bounds for min and max are both inclusive
unsigned int generateRandomNumber(int min, int max) {
    std::random_device seed;
    std::mt19937 rng(seed());
    std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);
    return dist(rng);
}

// This is only called by the first thread (the thread that keeps
// track of counting guests)
void checkCupcake() {
    while (currentCount < NUM_GUESTS) {
        mutex.lock();
        if (activeThreadIndex == 0) {
            // In this case, the cupcake will only be unavailable if someone
            // has eaten it, so we need to update the count and replace the cupcake
            if (!isCupcakeAvailable) {
                currentCount++;
                isCupcakeAvailable = true;
            }
            // Need to make sure this thread eats the cupcake once
            if (isCupcakeAvailable && !guestsPicked[0]) {
                currentCount++;
                isCupcakeAvailable = true;
                guestsPicked[0] = true;
            }
        }
        mutex.unlock();
    }
}

void navigateLabyrinth(unsigned int threadIndex) {
    while (currentCount < NUM_GUESTS) {
        mutex.lock();

        if (activeThreadIndex == threadIndex && isCupcakeAvailable && !guestsPicked[threadIndex]) {
            isCupcakeAvailable = false;
            guestsPicked[threadIndex] = true;
            std::cout << "Guest #" << threadIndex << " ate the cupcake!" << std::endl;
        }

        mutex.unlock();
    }
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    std::array<std::thread, NUM_GUESTS> threads{};
    // Designate the first thread as the counter. This thread will keep track
    // of whether the cupcake has been eaten
    threads[0] = std::thread(checkCupcake);
    for (size_t i = 1; i < threads.size(); i++) {
        threads[i] = std::thread(navigateLabyrinth, i);
    }
    // Keep picking guests at random until all guests have been counted
    while (currentCount < NUM_GUESTS) {
        activeThreadIndex = generateRandomNumber(0, NUM_GUESTS);
    }
    for (auto& thread : threads) {
        thread.join();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start);

    std::cout << "All " << currentCount << " guests have entered the labyrinth." << std::endl;
    std::cout << "Finished in " << duration.count() << "ms" << std::endl;
}