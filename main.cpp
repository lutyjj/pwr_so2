#include <iostream>
#include <ncurses.h>
#include <chrono>
#include <thread>
#include <vector>

using namespace std;

struct Car {
    int current_x = 0;
    int current_y = 0;
    int speed = 0;
};

void thread_func(Car *car) {
    for (int i = 0; i < 100; i++) {
        car->current_x = i;

        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }
}

int main() {

    vector<thread> threads;
    Car *cars[2];

    for (int i = 0; i < 2; i++) {
        cars[i] = new Car();
        threads.emplace_back(thread_func, ref(cars[i]));
    }

    initscr();
    while (true) {
        clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(33));

        for (int i = 0; i < 2; i++) {
            mvaddch(0, cars[i]->current_x, '.');
        }
        refresh();
    }

	endwin();

    for (std::thread & t : threads) {
        t.join();
    }
    return 0;
}