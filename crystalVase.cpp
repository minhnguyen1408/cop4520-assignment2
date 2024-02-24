//
// Created by Henry on 2/20/2024.
//
// Approach 1: Initially, the Minotaur opted for a strategy
// where all guests were allowed to enter the room simultaneously,
// leading to potential large crowds forming around the door.
// This approach poses a significant drawback as guests, particularly
// those at the back of the crowd, may never have an opportunity to
// witness the Minotaur's Crystal Vase.
//
//Approach 2: In the second strategy, the Minotaur implemented a system
// where guests could change a sign on the showroom door to either "BUSY" or "AVAILABLE,"
// indicating the presence of a guest in the room. I incorporated this solution in my program,
// along with a backoff function to address the potential issue of multiple guests attempting
// to set the sign simultaneously. This enhances program efficiency by increasing the likelihood
// that each guest will have an opportunity to view the Crystal Vase, unlike Approach 1.
// However, a disadvantage of this method is that insufficient waiting time may result in some
// guests missing the chance to see the vase, unlike the queue approach discussed later.
//
//Approach 3: The Minotaur's third strategy involved establishing a queue, where each exiting
// guest informs the next guest in line that the showroom is now available. This approach closely
// resembles a real-life scenario for fair access to the Crystal Vase, ensuring that each guest
// in the queue is guaranteed entry, unlike Approach 1. While Approach 2, with my backoff modification,
// approaches the fairness of Approach 3, experimentation and research suggest that the queue method
// is the fairest. The drawback of the queue method lies in the potential enlargement of the array as
// the number of guests increases in an array-based implementation.
#include <array>
#include <chrono>
#include <deque>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <unordered_set>

#define NUM_GUESTS 50

enum Status {
    AVAILABLE,
    BUSY
};

// Keeps track of what guests have already seen the vase
std::unordered_set<std::thread::id> guestsVisited{};
std::mutex mutex;
Status roomStatus = Status::AVAILABLE;

// Note: the bounds for min and max are both inclusive
unsigned int generateRandomNumber(int min, int max) {
    std::random_device seed;
    std::mt19937 rng(seed());
    std::uniform_int_distribution<std::mt19937::result_type> dist(min, max);
    return dist(rng);
}

void admireVase(int threadIndex) {
    std::thread::id threadId = std::this_thread::get_id();

    while (guestsVisited.size() < NUM_GUESTS) {
        mutex.lock();

        if (roomStatus == Status::AVAILABLE && guestsVisited.find(threadId) == guestsVisited.end()) {
            roomStatus = Status::BUSY;
            std::cout << "Guest #" << threadIndex << " is admiring the vase" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(generateRandomNumber(10, 500)));
            roomStatus = Status::AVAILABLE;

            guestsVisited.insert(threadId);
        }

        mutex.unlock();
    }
}

int main() {
    auto start = std::chrono::high_resolution_clock::now();
    std::array<std::thread, NUM_GUESTS> threads{};

    for (size_t i = 0; i < threads.size(); i++) {
        threads[i] = std::thread(admireVase, i);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end - start);

    std::cout << "All guests have viewed the vase." << std::endl;
    std::cout << "Finished in " << duration.count() << "ms" << std::endl;
}