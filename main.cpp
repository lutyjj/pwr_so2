#include <iostream>
#include <ncurses.h>
#include <chrono>
#include <thread>
#include <vector>

using namespace std;

struct Car
{
    int current_x = 0;
    int current_y = 0;
    int speed = 0;
    int number;
    thread *t;

    Car(int number, int speed)
    {
        this->number = number;
        this->speed = speed;

        t = new thread([this]() { thread_func(); });
    }

    void thread_func()
    {
        for (int i = 0; i < 100; i++)
        {
            this->current_x = i / speed;

            std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
    }
};

int main()
{
    initscr();

    vector<Car *> cars;

    for (int i = 0; i < 3; i++)
    {
        cars.push_back(new Car(i, i+1));
    }

    while (true)
    {
        clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(33));

        for (auto car : cars)
        {
            mvprintw(0, car->current_x, "%d", car->number);
        }
        refresh();
    }

    endwin();
    for (auto car : cars)
    {
        car->t->join();
        delete car;
    }

    cars.clear();

    return 0;
}