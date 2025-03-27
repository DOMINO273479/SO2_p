#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <random>
#include <chrono>

using namespace std;

class DiningPhilosophers {
private:
    int num_philosophers;
    vector<thread> philosophers;
    vector<mutex> forks;
    mutex output_mutex;

    void philosopher(int id) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(1000, 3000);
        
        while (true) {
            // Thinking
            log_status(id, "is thinking");
            this_thread::sleep_for(chrono::milliseconds(dist(gen)));
            
            // Hungry
            log_status(id, "is hungry");
            
            // Pick up forks (left first, then right)
            int left = id;
            int right = (id + 1) % num_philosophers;
            
            if (id % 2 == 0) {
                forks[left].lock();
                forks[right].lock();
            } else {
                forks[right].lock();
                forks[left].lock();
            }
            
            // Eating
            log_status(id, "is eating");
            this_thread::sleep_for(chrono::milliseconds(dist(gen)));
            
            // Put down forks
            forks[left].unlock();
            forks[right].unlock();
        }
    }

    void log_status(int id, const string &status) {
        lock_guard<mutex> lock(output_mutex);
        cout << "Philosopher " << id << " " << status << "" << endl;
    }

public:
    DiningPhilosophers(int n) : num_philosophers(n), forks(n) {}

    void start() {
        for (int i = 0; i < num_philosophers; ++i) {
            philosophers.emplace_back(&DiningPhilosophers::philosopher, this, i);
        }
        for (auto &philosopher : philosophers) {
            philosopher.join();
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <number_of_philosophers>" << endl;
        return 1;
    }
    
    int num_philosophers = stoi(argv[1]);
    DiningPhilosophers dp(num_philosophers);
    dp.start();
    
    return 0;
}
