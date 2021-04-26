#include <ncurses.h>
#include <random>
#include <vector>
#include "Road.h"
#include "Car.h"

Road::Road(int x, int y)
{
    this->x = x;
    this->y = y;
    this->stop_flag = false;

    t_spawn_car = new thread([this]() { spawn_car(); });
}

void Road::draw()
{
    rectangle(0, 0, y - 1, x - 1);
    rectangle(2, 4, y - 3, x - 5);
}

void Road::rectangle(int y1, int x1, int y2, int x2)
{
    mvhline(y1, x1, 0, x2 - x1);
    mvhline(y2, x1, 0, x2 - x1);
    mvvline(y1, x1, 0, y2 - y1);
    mvvline(y1, x2, 0, y2 - y1);
    mvaddch(y1, x1, ACS_ULCORNER);
    mvaddch(y2, x1, ACS_LLCORNER);
    mvaddch(y1, x2, ACS_URCORNER);
    mvaddch(y2, x2, ACS_LRCORNER);
}

void Road::spawn_car()
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<> dist(500, 3000);

    int count = 0;
    while (!this->stop_flag && cars.size() < 5)
    {
        count++;
        cars.push_back(new Car(count, this));
        std::this_thread::sleep_for(std::chrono::milliseconds(dist(rng)));
    }
}