#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <random>
#include <memory>
#include <algorithm>

using namespace std;

class DiningPhilosophers {
private:
    int num_philosophers; // Number of philosophers
    vector<thread> philosophers; // Vector of philosopher threads
    vector<unique_ptr<atomic<bool>>> forks; // Atomic boolean flags 
    atomic<bool> running; // Flag to control simulation execution
    atomic<int> console_lock; // Lock to prevent mixed console output

    // Function executed by each philosopher thread
    void philosopher(int id) {
        random_device rd;
        mt19937 gen(rd()); // Random number generator
        uniform_int_distribution<> dist(1000, 3000); // Random thinking/eating time

        while (running.load()) { // Repeat until simulation ends
            log_status(id, "is thinking");
            this_thread::sleep_for(chrono::milliseconds(dist(gen))); // Thinking

            log_status(id, "is hungry");

            // Determine fork order to prevent deadlock
            int left = id;
            int right = (id + 1) % num_philosophers;
            int first, second;

            if (id % 2 == 0) { // Philosophers take left fork first
                first = left;
                second = right;
            } else { // Other philosophers take right fork first
                first = right;
                second = left;
            }

            // Pick up first fork (busy wait if unavailable)
            while (!forks[first]->exchange(false)) {
                this_thread::yield(); // Yield CPU to avoid wasting cycles
            }
            // Pick up second fork (busy wait if unavailable)
            while (!forks[second]->exchange(false)) {
                this_thread::yield();
            }

            log_status(id, "is eating");
            this_thread::sleep_for(chrono::milliseconds(dist(gen))); // Eating

            // Put down forks
            forks[first]->store(true);
            forks[second]->store(true);
        }
    }

    // Thread-safe logging function
    void log_status(int id, const string& status) {
        while (console_lock.exchange(1)) { // Lock console
            this_thread::yield();
        }
        cout << "Philosopher " << id << " " << status << endl;
        console_lock.store(0); // Unlock console
    }

public:
    // Constructor: Initializes philosophers and forks
    DiningPhilosophers(int n) : num_philosophers(n), running(true), console_lock(0) {
        for (int i = 0; i < n; ++i) {
            forks.emplace_back(make_unique<atomic<bool>>(true)); // All forks are initially available
        }
    }

    // Starts the simulation
    void start() {
        for (int i = 0; i < num_philosophers; ++i) {
            philosophers.emplace_back(&DiningPhilosophers::philosopher, this, i); // Create philosopher threads
        }

        this_thread::sleep_for(chrono::seconds(40)); // Run for 40 seconds
        running.store(false); // Stop simulation

        for (auto &philosopher : philosophers) {
            philosopher.join(); // Wait for all threads to finish
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <number_of_philosophers>" << endl;
        return 1;
    }

    int num_philosophers;
    try {
        num_philosophers = stoi(argv[1]); // Convert input to integer
        if (num_philosophers <= 1) {
            throw invalid_argument("Number of philosophers must be greater than 1.");
        }
    } catch (const exception &e) {
        cerr << "Invalid input: " << e.what() << endl;
        return 1;
    }

    DiningPhilosophers dp(num_philosophers); 
    dp.start(); // Start simulation

    return 0;
}
